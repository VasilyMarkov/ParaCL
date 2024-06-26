#include "ast.hpp"

int evalVisitor::visit(const scopeNode& node) 
{
    node.left_->eval(*this);
    node.right_->eval(*this);
    return 0;
}

int evalVisitor::visit(const ifNode& node)
{
    if (node.expr_ != nullptr) {
        if (node.expr_->eval(*this) && node.left_ != nullptr) {
            node.left_->eval(*this);
        }
        else if (!node.expr_->eval(*this) && node.right_ != nullptr) {
            node.right_->eval(*this);
        }
    }
    return 0;
}

int evalVisitor::visit(const whileNode& node)
{
    if(node.left_ != nullptr) {
        while (node.left_->eval(*this) && node.right_ != nullptr) {
            node.right_->eval(*this);
        }    
    }
    return 0;
}

int evalVisitor::visit(const notNode& node)
{
    return !node.left_->eval(*this);
}

int evalVisitor::visit(const minusNode& node)
{
    return -node.left_->eval(*this);
}

int evalVisitor::visit(const arithNode& node)
{
    int left_value = node.left_ ? node.left_->eval(*this): 0;
    int right_value = node.right_ ? node.right_->eval(*this): 0;

    return node.operations_.at(node.op_)(left_value, right_value);
}

int evalVisitor::visit(const predNode& node)
{
    int left_value = node.left_ ? node.left_->eval(*this): 0;
    int right_value = node.right_ ? node.right_->eval(*this): 0;

    return node.predicates_.at(node.op_)(left_value, right_value);
}

int evalVisitor::visit(const numNode& node)
{
    return node.value_;
}

int evalVisitor::visit(const varNode& node)
{
    return var_store.at(node.id_);
}

int evalVisitor::visit(const inputNode& node)
{
    int input = 0;
    std::cin >> input;

    return input;
}

int evalVisitor::visit(const outputNode& node)
{
    std::cout << node.id_ << " = " << var_store.at(node.id_) << std::endl;

    return 0;
}

int evalVisitor::visit(const assignNode& node)
{
    auto var = std::static_pointer_cast<varNode>(node.left_);
    var_store.at(var->name()) = node.right_->eval(*this);

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
