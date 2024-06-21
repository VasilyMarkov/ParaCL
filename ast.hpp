#pragma once
#include <iostream>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

inline void print(const std::unordered_map<std::string, int>& cont) {
    for (auto&& elem : cont) {
        std::cout << elem.first << " = " << elem.second << std::endl;
    }
}

inline std::unordered_map<std::string, int> var_store; //global variables

enum class node_t{
    op,
    number,
    assign
};

enum class arith_t{
    PLUS,
    MINUS,
    MULT,
    DIV
};

enum class pred_t{
    GR,
    GRE,
    LW,
    LWE,
    EQ,
    NEQ,
    AND,
    OR
};

class iNode {    
protected:
    std::shared_ptr<iNode> left_ = nullptr;
    std::shared_ptr<iNode> right_ = nullptr;
public:
    iNode() = default;
    explicit iNode(std::shared_ptr<iNode> left, std::shared_ptr<iNode> right): 
        left_(left), 
        right_(right) {}
    
    virtual ~iNode(){}

    virtual int eval() const = 0;
    virtual void dump(int indent = 0) const = 0;
    
    friend void dumpTree(std::shared_ptr<iNode> node, int indent = 0) {
        if (node == nullptr) return;   
        dumpTree(node->left_, indent + 4);
        node->dump(indent);
        dumpTree(node->right_, indent + 4);
    }
};

class scopeNode: public iNode {
public:
    explicit scopeNode(std::shared_ptr<iNode> right, std::shared_ptr<iNode> left): 
        iNode(right, left) {}
    int eval() const override {
        left_->eval();
        right_->eval();
        return 0;
    }
    void dump(int indent = 0) const override {
        // std::cout << std::string(indent, ' ') << "SCOPE: ";
    }
};

class ifNode: public iNode {
   std::shared_ptr<iNode> expr_ = nullptr; 
public:
    explicit ifNode(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> if_stmt, std::shared_ptr<iNode> else_stmt): 
        expr_(expr), 
        iNode(if_stmt, else_stmt) {}

    int eval() const override {
        if (expr_ != nullptr && expr_->eval() && left_ != nullptr) {
            left_->eval();
        }
        else if (expr_ != nullptr && !expr_->eval() && right_ != nullptr) {
            std::cout << "false" << std::endl;
            right_->eval();
        }
        return 0;
    }
    void dump(int indent = 0) const override {
        // std::cout << std::string(indent, ' ') << "If: " << std::endl;
    }
};


class whileNode: public iNode {
public:
    explicit whileNode(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> stmt): 
        iNode(expr, stmt) {}

    int eval() const override {
        if(left_ != nullptr) {
            while (left_->eval() && right_ != nullptr) {
                right_->eval();
            }    
        }
        return 0;
    }
    void dump(int indent = 0) const override {
        // std::cout << std::string(indent, ' ') << "While: ";
    }
};

class notNode: public iNode {
public:
    notNode() = default;
    explicit notNode(std::shared_ptr<iNode> left, std::shared_ptr<iNode> right): iNode(left, right) {std::cout << "not" << std::endl;}
    int eval() const override {
        return !left_->eval();
    }
    void dump(int indent = 0) const override {
        // std::cout << std::string(indent, ' ') << "While: ";
    }
};

class arithNode: public iNode { 
    arith_t op_;
    inline static const std::unordered_map<arith_t, std::function<int(int, int)>> operations_ = {
        {arith_t::PLUS,  {[](int lhs, int rhs) {return lhs + rhs;}}},
        {arith_t::MINUS, {[](int lhs, int rhs) {return lhs - rhs;}}},
        {arith_t::MULT,  {[](int lhs, int rhs) {return lhs * rhs;}}},
        {arith_t::DIV,   {[](int lhs, int rhs) {return lhs / rhs;}}},
    };
    inline static const std::unordered_map<arith_t, std::function<void()>> dumped_arith_ = {
        {arith_t::PLUS,  {[]() {std::cout << "+" << std::endl;}}},
        {arith_t::MINUS, {[]() {std::cout << "-" << std::endl;}}},
        {arith_t::MULT,  {[]() {std::cout << "*" << std::endl;}}},
        {arith_t::DIV,   {[]() {std::cout << "/" << std::endl;}}},
    };
public:
    explicit arithNode(arith_t op, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right): 
        op_(op), 
        iNode(left, right) {}
    int eval() const override {
        int left_value = left_ ? left_->eval(): 0.0;
        int right_value = right_ ? right_->eval(): 0.0;

        return operations_.at(op_)(left_value, right_value);
    }
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "OP: ";
        dumped_arith_.at(op_)();
    }
};

class predNode: public iNode { 
    pred_t op_;
    inline static const std::unordered_map<pred_t, std::function<int(int, int)>> predicates_ = {
        {pred_t::GR,  {[](int lhs, int rhs) {return lhs >  rhs;}}},
        {pred_t::GRE, {[](int lhs, int rhs) {return lhs >= rhs;}}},
        {pred_t::LW,  {[](int lhs, int rhs) {return lhs <  rhs;}}},
        {pred_t::LWE, {[](int lhs, int rhs) {return lhs <= rhs;}}},
        {pred_t::EQ,  {[](int lhs, int rhs) {return lhs == rhs;}}},
        {pred_t::NEQ, {[](int lhs, int rhs) {return lhs != rhs;}}},
        {pred_t::AND, {[](int lhs, int rhs) {return lhs && rhs;}}},
        {pred_t::OR,  {[](int lhs, int rhs) {return lhs || rhs;}}}
    };
    inline static const std::unordered_map<pred_t, std::function<void()>> dumped_pred_ = {
        {pred_t::GR,  {[]() {std::cout << ">"  << std::endl;}}},
        {pred_t::GRE, {[]() {std::cout << ">=" << std::endl;}}},
        {pred_t::LW,  {[]() {std::cout << "<"  << std::endl;}}},
        {pred_t::LWE, {[]() {std::cout << "<=" << std::endl;}}},
        {pred_t::EQ,  {[]() {std::cout << "==" << std::endl;}}},
        {pred_t::NEQ, {[]() {std::cout << "!=" << std::endl;}}},
        {pred_t::AND, {[]() {std::cout << "&&" << std::endl;}}},
        {pred_t::OR,  {[]() {std::cout << "||" << std::endl;}}}
    };
public:
    explicit predNode(pred_t op, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right) noexcept: 
        op_(op), 
        iNode(left, right) {}

    int eval() const override {
        int left_value = left_ ? left_->eval(): 0.0;
        int right_value = right_ ? right_->eval(): 0.0;

        return predicates_.at(op_)(left_value, right_value);
    }
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "OP: ";
        dumped_pred_.at(op_)();
    }
};

class numNode: public iNode { 
    int value_;
public:
    explicit numNode(int value): 
        value_(value), 
        iNode(nullptr, nullptr) {}

    int eval() const override {
        return value_;
    }
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Num: " << value_ << std::endl;
    }
};

class varNode: public iNode { 
    std::string id_;
public:
    explicit varNode(const std::string& id): id_(id), iNode(nullptr, nullptr) {
        var_store.emplace(id_, 0);
    }

    std::string name() const {return id_;}    

    int eval() const override {    
        return var_store.at(id_);

    }
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Var: " << id_ << std::endl;
    }
};

class assignNode: public iNode { 
public:
    explicit assignNode(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> var): iNode(expr, var) {}

    int eval() const override {
        auto var = std::static_pointer_cast<varNode>(left_);
        var_store.at(var->name()) = right_->eval();
        return 0;
    }
    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "=" << std::endl;
    }
};

inline std::shared_ptr<iNode> newScope(std::shared_ptr<iNode> left, std::shared_ptr<iNode> right) {
    return std::make_shared<scopeNode>(left, right);
}

inline std::shared_ptr<iNode> newIf(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> if_stmt, std::shared_ptr<iNode> else_stmt) {
    return std::make_shared<ifNode>(expr, if_stmt, else_stmt);
}

inline std::shared_ptr<iNode> newWhile(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> stmt) {
    return std::make_shared<whileNode>(expr, stmt);
}

inline std::shared_ptr<iNode> newNot(std::shared_ptr<iNode> expr) {
    return std::make_shared<notNode>(expr, nullptr);
}

inline std::shared_ptr<iNode> newArith(arith_t type, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right) {
    return std::make_shared<arithNode>(type, left, right);
}

inline std::shared_ptr<iNode> newPred(pred_t type, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right) {
    return std::make_shared<predNode>(type, left, right);
}

inline std::shared_ptr<iNode> newNumber(int value) {
    return std::make_shared<numNode>(value);
}

inline std::shared_ptr<iNode> newVar(std::string& id) {
    return std::make_shared<varNode>(id);
}

inline std::shared_ptr<iNode> newAssign(std::shared_ptr<iNode> expr, std::shared_ptr<iNode> var) {
    return std::make_shared<assignNode>(expr, var);
}

