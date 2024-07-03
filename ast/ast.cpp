#include "ast.hpp"
#include <sstream>
#include <string>

namespace ast {

int EvalVisitor::visit(const scopeNode& node) 
{
    node.left_->eval(*this);
    node.right_->eval(*this);
    return 0;
}

int EvalVisitor::visit(const ifNode& node)
{
    if (node.expr_ != nullptr) {
        if (node.left_ != nullptr && node.expr_->eval(*this)) {
            node.left_->eval(*this);
        }
        else if (node.right_ != nullptr && !node.expr_->eval(*this)) {
            node.right_->eval(*this);
        }
    }
    return 0;
}

int EvalVisitor::visit(const whileNode& node)
{
    if(node.left_ != nullptr) {
        while (node.left_->eval(*this) && node.right_ != nullptr) {
            node.right_->eval(*this);
        }    
    }
    return 0;
}

int EvalVisitor::visit(const notNode& node)
{
    return !node.left_->eval(*this);
}

int EvalVisitor::visit(const minusNode& node)
{
    return -node.left_->eval(*this);
}

int EvalVisitor::visit(const arithNode& node)
{
    int left_value = node.left_ ? node.left_->eval(*this): 0;
    int right_value = node.right_ ? node.right_->eval(*this): 0;

    return node.operations_.at(node.op_)(left_value, right_value);
}

int EvalVisitor::visit(const predNode& node)
{
    int left_value = node.left_ ? node.left_->eval(*this): 0;
    int right_value = node.right_ ? node.right_->eval(*this): 0;

    return node.predicates_.at(node.op_)(left_value, right_value);
}

int EvalVisitor::visit(const numNode& node)
{
    return node.value_;
}

int EvalVisitor::visit(const varNode& node)
{
    if (!global_store_.contains(node.id_)) {
        std::stringstream ss;
        ss << "The variable: " << node.id_ << " doesn't exist.";
        throw std::runtime_error(ss.str());
    }
    return global_store_.at(node.id_);
}

int EvalVisitor::visit(const inputNode& node)
{
    int input = 0;
    std::cin >> input;
    return input;
}

int EvalVisitor::visit(const outputNode& node)
{
    std::cout << node.left_->eval(*this);
    return 0;
}

int EvalVisitor::visit(const assignNode& node)
{
    if (auto var = dynamic_cast<varNode*>(node.left_.get())) {
        if (global_store_.contains(var->name())) {
            global_store_.at(var->name()) = node.right_->eval(*this);
        }
        else {
            global_store_.emplace(var->name(), node.right_->eval(*this));
        }
    }
    return 0;
}

int DumpVisitor::visit(const scopeNode& node)
{
    std::cout << std::string(indent_, ' ') << "Scope:" << std::endl;
    node.left_->eval(*this);
    node.right_->eval(*this);
    return 0;
}

int DumpVisitor::visit(const ifNode& node)
{
    std::cout << std::string(indent_, ' ') << "If:" << std::endl;
    DumpVisitor if_visitor(indent_ + 2);
    node.expr_->eval(if_visitor);
    node.left_->eval(if_visitor);
    if (node.right_ != nullptr) {
        std::cout << std::string(indent_ + 2, ' ') << "Else:" << std::endl;
        node.right_->eval(if_visitor);
    }
    return 0;
}

int DumpVisitor::visit(const whileNode& node)
{
    std::cout << std::string(indent_, ' ') << "While:" << std::endl;
    DumpVisitor while_visitor(indent_ + 2);
    node.left_->eval(while_visitor);
    node.right_->eval(while_visitor);
    return 0;
}

int DumpVisitor::visit(const notNode& node)
{
    std::cout << std::string(indent_, ' ') << "!" << std::endl;
    DumpVisitor not_visitor(indent_ + 2);
    node.left_->eval(not_visitor);
    return 0;
}

int DumpVisitor::visit(const minusNode& node)
{
    std::cout << std::string(indent_, ' ') << "-" << std::endl;
    DumpVisitor minus_visitor(indent_ + 2);
    node.left_->eval(minus_visitor);
    return 0;
}

int DumpVisitor::visit(const arithNode &node)
{
    DumpVisitor arith_visitor(indent_ + 2);
    node.left_->eval(arith_visitor);
    node.dumped_arith_.at(node.op_)();
    node.right_->eval(arith_visitor);
    return 0;
}

int DumpVisitor::visit(const predNode& node)
{
    DumpVisitor pred_visitor(indent_ + 2);
    node.left_->eval(pred_visitor);
    node.dumped_pred_.at(node.op_)();
    node.right_->eval(pred_visitor);
    return 0;
}

int DumpVisitor::visit(const numNode& node)
{
    std::cout << std::string(indent_, ' ') << "Num:" << node.value_ << std::endl;
    return 0;
}

int DumpVisitor::visit(const varNode& node)
{
    std::cout << std::string(indent_, ' ') << "Var:" << node.name() << std::endl;
    return 0;
}

int DumpVisitor::visit(const inputNode& node)
{
    std::cout << std::string(indent_, ' ') << "Input:" << std::endl;
    return 0;
}

int DumpVisitor::visit(const outputNode& node)
{
    std::cout << std::string(indent_, ' ') << "Output: " << node.id_ << std::endl;
    return 0;
}

int DumpVisitor::visit(const assignNode& node)
{
    DumpVisitor assign_visitor(indent_ + 2);
    node.left_->eval(assign_visitor);
    std::cout << std::string(indent_, ' ') << "=" << std::endl;
    node.right_->eval(assign_visitor);
    return 0;
}

int scopeNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int ifNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int whileNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int notNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int minusNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int arithNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int predNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int numNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int varNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int inputNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int outputNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

int assignNode::eval(Visitor& visitor) const
{
    return visitor.visit(*this);
}

}