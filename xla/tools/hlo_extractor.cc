/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/tools/hlo_extractor.h"

#include <unistd.h>

#include <deque>
#include <memory>
#include <utility>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "xla/hlo/ir/dfs_hlo_visitor_with_default.h"
#include "xla/hlo/ir/hlo_clone_context.h"
#include "xla/hlo/ir/hlo_computation.h"
#include "xla/hlo/ir/hlo_instruction.h"
#include "xla/service/compilation_environments.h"
#include "xla/service/hlo_verifier.h"
#include "xla/status.h"
#include "xla/tests/test_utils.h"
#include "tsl/platform/status.h"

namespace xla {
namespace {

// Visitor that build a new HLO module with an entry computation and a root that
// is provided to the visit function. Only HLOs that are reachable from the new
// root instruction are included in the new module.
//
// The constructor allows specifying a set of boundary HLOs to prune the HLO
// graph. HLOs at the boundary are replaced with parameters. Can be nullptr
// which means no boundary, i.e. no HLOs are replaced with parameters.
class ExtractionVisitor : public ConstDfsHloVisitorWithDefault {
 public:
  explicit ExtractionVisitor(
      const HloModule& old_module,
      absl::flat_hash_set<const HloInstruction*>* boundary,
      ExtractSelector extract_selector,
      ReplaceTypeSelector replace_type_selector)
      : old_module_(old_module),
        module_(std::make_unique<HloModule>(
            "extracted", config_,
            std::make_unique<CompilationEnvironments>(old_module.comp_envs()))),
        clone_context_(module_.get()),
        builder_("entry_computation"),
        boundary_(boundary),
        extract_selector_(extract_selector),
        replace_type_selector_(replace_type_selector) {}

  Status HandleParameter(const HloInstruction* parameter) override {
    // Entry parameters need renumbering.
    return ReplaceWithParameter(parameter);
  }

  Status DefaultAction(const HloInstruction* hlo) override {
    // Replace the following two types of instructions with parameters/constants
    // (1) the instructions at the boundary with (2) the instructions that are
    // not selected by the hlo_selector.
    if ((boundary_ != nullptr && boundary_->contains(hlo) > 0) ||
        (extract_selector_ != nullptr && !extract_selector_(hlo))) {
      if (replace_type_selector_ != nullptr) {
        switch (replace_type_selector_(hlo)) {
          case ReplaceType::kReplaceConst:
            return ReplaceWithConstant(hlo);
          case ReplaceType::kReplaceParam:
            return ReplaceWithParameter(hlo);
          default:
            QCHECK(false) << "Unsupported replacement type";
        }
      }

      return ReplaceWithParameter(hlo);
    }
    std::vector<HloInstruction*> new_operands;
    for (auto operand : hlo->operands()) {
      new_operands.push_back(clone_context_.GetInstruction(operand));
    }
    auto instruction =
        hlo->CloneWithNewOperands(hlo->shape(), new_operands, &clone_context_);
    builder_.AddInstruction(std::move(instruction));
    return OkStatus();
  }

  Status FinishVisit(const HloInstruction* /*root*/) override {
    module_->AddEntryComputation(builder_.Build());
    // Rename HLOs so that their name matches the original. By default,
    // HLOs get new unique names when adding a new entry computation to
    // a module.
    for (auto computation : old_module_.MakeComputationPostOrder()) {
      for (auto old_instruction : computation->MakeInstructionPostOrder()) {
        if (auto new_instruction =
                clone_context_.FindInstruction(old_instruction)) {
          new_instruction->SetAndSanitizeName(old_instruction->name());
        }
      }
    }
    return OkStatus();
  }

  HloModule* module() { return module_.get(); }

  std::unique_ptr<HloModule> ConsumeModule() { return std::move(module_); }

 private:
  // Replace the `hlo` with Constant of the same shape.
  Status ReplaceWithConstant(const HloInstruction* hlo) {
    StatusOr<Literal> literal_status = MakeFakeLiteral(hlo->shape());
    TF_CHECK_OK(literal_status.status());
    auto new_const =
        HloInstruction::CreateConstant(std::move(literal_status.value()));
    clone_context_.MapInstruction(hlo, new_const.get());
    builder_.AddInstruction(std::move(new_const));
    return OkStatus();
  }

  // Replace the `hlo` with Parameter of the same shape.
  Status ReplaceWithParameter(const HloInstruction* hlo) {
    auto new_parameter = HloInstruction::CreateParameter(
        parameter_number_, hlo->shape(), hlo->name());
    parameter_number_++;
    clone_context_.MapInstruction(hlo, new_parameter.get());
    builder_.AddInstruction(std::move(new_parameter));
    return OkStatus();
  }

  const HloModule& old_module_;
  HloModuleConfig config_;
  std::unique_ptr<HloModule> module_;
  HloCloneContext clone_context_;
  HloComputation::Builder builder_;
  absl::flat_hash_set<const HloInstruction*>* boundary_;
  ExtractSelector extract_selector_;
  ReplaceTypeSelector replace_type_selector_;
  int64_t parameter_number_ = 0;
};

void ComputeBoundary(const HloInstruction* root, int64_t limit,
                     absl::flat_hash_set<const HloInstruction*>* boundary) {
  std::deque<const HloInstruction*> worklist;
  absl::flat_hash_map<const HloInstruction*, int64_t> visited;
  worklist.push_back(root);
  visited.emplace(root, 0);
  while (!worklist.empty()) {
    auto hlo = worklist.front();
    worklist.pop_front();
    int64_t hops = visited[hlo];
    if (hops > limit) {
      boundary->insert(hlo);
      continue;
    }
    for (const HloInstruction* operand : hlo->operands()) {
      if (visited.count(operand)) {
        continue;
      }
      worklist.push_back(operand);
      visited.emplace(operand, hops + 1);
    }
  }
}

}  // namespace

std::unique_ptr<HloModule> ExtractModule(
    HloInstruction* instruction, int64_t height,
    ExtractSelector extract_selector,
    ReplaceTypeSelector replace_type_selector) {
  absl::flat_hash_set<const HloInstruction*> boundary;
  if (height != -1) {
    ComputeBoundary(instruction, height, &boundary);
  }
  ExtractionVisitor visitor(*instruction->GetModule(), &boundary,
                            extract_selector, replace_type_selector);
  TF_CHECK_OK(instruction->Accept(&visitor));

  // The first pass may leave unused parameter instructions. Do another
  // extraction pass to remove unused parameters. This is done because
  // HloComputation does not allow removing parameters after the computation has
  // been built.
  ExtractionVisitor cleanup_visitor(*visitor.module(), /*boundary=*/nullptr,
                                    /*extract_selector=*/nullptr,
                                    /*replace_type_selector=*/nullptr);
  TF_CHECK_OK(visitor.module()->entry_computation()->root_instruction()->Accept(
      &cleanup_visitor));

  HloVerifier verifier(/*layout_sensitive=*/false,
                       /*allow_mixed_precision=*/true);
  TF_CHECK_OK(verifier.Run(cleanup_visitor.module()).status());
  return cleanup_visitor.ConsumeModule();
}

}  // namespace xla
