// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_COMPILER_SOURCE_POSITION_H_
#define V8_COMPILER_SOURCE_POSITION_H_

#include "src/base/compiler-specific.h"
#include "src/compiler/node-aux-data.h"
#include "src/globals.h"

namespace v8 {
namespace internal {
namespace compiler {

// Encapsulates encoding and decoding of sources positions from which Nodes
// originated.
class SourcePosition final {
 public:
  explicit SourcePosition(int raw = kUnknownPosition) : raw_(raw) {}

  static SourcePosition Unknown() { return SourcePosition(kUnknownPosition); }
  bool IsUnknown() const { return raw() == kUnknownPosition; }
  bool IsKnown() const { return raw() != kUnknownPosition; }

  int raw() const { return raw_; }

 private:
  static const int kUnknownPosition = kNoSourcePosition;
  int raw_;
};


inline bool operator==(const SourcePosition& lhs, const SourcePosition& rhs) {
  return lhs.raw() == rhs.raw();
}

inline bool operator!=(const SourcePosition& lhs, const SourcePosition& rhs) {
  return !(lhs == rhs);
}

class V8_EXPORT_PRIVATE SourcePositionTable final
    : public NON_EXPORTED_BASE(ZoneObject) {
 public:
  class Scope final {
   public:
    Scope(SourcePositionTable* source_positions, SourcePosition position)
        : source_positions_(source_positions),
          prev_position_(source_positions->current_position_) {
      Init(position);
    }
    Scope(SourcePositionTable* source_positions, Node* node)
        : source_positions_(source_positions),
          prev_position_(source_positions->current_position_) {
      Init(source_positions_->GetSourcePosition(node));
    }
    ~Scope() { source_positions_->current_position_ = prev_position_; }

   private:
    void Init(SourcePosition position) {
      if (position.IsKnown()) source_positions_->current_position_ = position;
    }

    SourcePositionTable* const source_positions_;
    SourcePosition const prev_position_;
    DISALLOW_COPY_AND_ASSIGN(Scope);
  };

  explicit SourcePositionTable(Graph* graph);

  void AddDecorator();
  void RemoveDecorator();

  SourcePosition GetSourcePosition(Node* node) const;
  void SetSourcePosition(Node* node, SourcePosition position);

  void set_current_position(int position) {
    current_position_ = SourcePosition(position);
  }

  void Print(std::ostream& os) const;

 private:
  class Decorator;

  Graph* const graph_;
  Decorator* decorator_;
  SourcePosition current_position_;
  NodeAuxData<SourcePosition> table_;

  DISALLOW_COPY_AND_ASSIGN(SourcePositionTable);
};

}  // namespace compiler
}  // namespace internal
}  // namespace v8

#endif  // V8_COMPILER_SOURCE_POSITION_H_
