#pragma once
#include <iostream>
#include <cmath>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

inline void print(const std::unordered_map<std::string, int>& cont) {
    for (auto&& elem : cont) {
        std::cout << elem.first << " = " << elem.second << std::endl;
    }
}

inline std::unordered_map<std::string, int> var_store; 

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
    NEQ
};

class iNode {    
protected:
    std::shared_ptr<iNode> left_;
    std::shared_ptr<iNode> right_;
public:
    iNode(){}
    iNode(std::shared_ptr<iNode> left, std::shared_ptr<iNode> right): 
        left_(left), 
        right_(right) {}
    
    virtual ~iNode(){}

    virtual int eval() const = 0;
    virtual void dump(int) const = 0;
    
    friend void dumpTree(std::shared_ptr<iNode> node, int indent) {
        if (node == nullptr) return;   
        dumpTree(node->left_, indent + 4);
        node->dump(indent);
        dumpTree(node->right_, indent + 4);
    }
};


class arithNode: public iNode { 
    arith_t op_;
    inline static const std::unordered_map<arith_t, std::function<int(int, int)>> operations = {
        {arith_t::PLUS,  {[](int lhs, int rhs) {return lhs + rhs;}}},
        {arith_t::MINUS, {[](int lhs, int rhs) {return lhs - rhs;}}},
        {arith_t::MULT,  {[](int lhs, int rhs) {return lhs * rhs;}}},
        {arith_t::DIV,   {[](int lhs, int rhs) {return lhs / rhs;}}},
    };
    inline static const std::unordered_map<arith_t, std::function<void()>> dumped_arith = {
        {arith_t::PLUS,  {[]() {std::cout << "+" << std::endl;}}},
        {arith_t::MINUS, {[]() {std::cout << "-" << std::endl;}}},
        {arith_t::MULT,  {[]() {std::cout << "*" << std::endl;}}},
        {arith_t::DIV,   {[]() {std::cout << "/" << std::endl;}}},
    };
public:
    arithNode(arith_t op, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right): 
        op_(op), 
        iNode(left, right) {}
    int eval() const override {
        int left_value = left_ ? left_->eval(): 0.0;
        int right_value = right_ ? right_->eval(): 0.0;

        return operations.at(op_)(left_value, right_value);
    }
    void dump(int indent) const override {
        std::cout << std::string(indent, ' ') << "OP: ";
        dumped_arith.at(op_)();
    }
};

class predNode: public iNode { 
    pred_t op_;
    inline static const std::unordered_map<pred_t, std::function<int(int, int)>> predicates = {
        {pred_t::GR,  {[](int lhs, int rhs) {return lhs >  rhs;}}},
        {pred_t::GRE, {[](int lhs, int rhs) {return lhs >= rhs;}}},
        {pred_t::LW,  {[](int lhs, int rhs) {return lhs <  rhs;}}},
        {pred_t::LWE, {[](int lhs, int rhs) {return lhs <= rhs;}}},
        {pred_t::EQ,  {[](int lhs, int rhs) {return lhs == rhs;}}},
        {pred_t::NEQ, {[](int lhs, int rhs) {return lhs != rhs;}}}
    };
    inline static const std::unordered_map<pred_t, std::function<void()>> dumped_pred = {
        {pred_t::GR,  {[]() {std::cout << ">"  << std::endl;}}},
        {pred_t::GRE, {[]() {std::cout << ">=" << std::endl;}}},
        {pred_t::LW,  {[]() {std::cout << "<"  << std::endl;}}},
        {pred_t::LWE, {[]() {std::cout << "<=" << std::endl;}}},
        {pred_t::EQ,  {[]() {std::cout << "==" << std::endl;}}},
        {pred_t::NEQ, {[]() {std::cout << "!=" << std::endl;}}}
    };
public:
    predNode(pred_t op, std::shared_ptr<iNode> left, std::shared_ptr<iNode> right) noexcept: 
        op_(op), 
        iNode(left, right) {}

    int eval() const override {
        int left_value = left_ ? left_->eval(): 0.0;
        int right_value = right_ ? right_->eval(): 0.0;

        return predicates.at(op_)(left_value, right_value);

    }
    void dump(int indent) const override {
        std::cout << std::string(indent, ' ') << "OP: ";
        dumped_pred.at(op_)();
    }
};

class numNode: public iNode { 
    int value_;
public:
    numNode(int value): 
        value_(value), 
        iNode(nullptr, nullptr) {}

    int eval() const override {
        return value_; 
    }
    void dump(int indent) const override {
        std::cout << std::string(indent, ' ') << "Num: " << value_ << std::endl;
    }
};

class varNode: public iNode { 
    std::string id_;
public:
    varNode(const std::string& id): id_(id), iNode(nullptr, nullptr) {var_store.emplace(id_, 0);}

    std::string name() const {return id_;}    

    int eval() const override {
        return 0;
    }
    void dump(int indent) const override {
        std::cout << std::string(indent, ' ') << "Var: " << id_ << std::endl;
    }
};

class assignNode: public iNode { 
public:
    assignNode(std::shared_ptr<iNode> var, std::shared_ptr<iNode> expr): iNode(var, expr) {}

    int eval() const override {
        auto var_name = std::static_pointer_cast<varNode>(left_)->name();
        var_store.at(var_name) = right_->eval();
        return 0;
    }
    void dump(int indent) const override {
        std::cout << std::string(indent, ' ') << "=" << std::endl;
    }
};

inline int eval(std::shared_ptr<iNode> node) {
    if (node == nullptr)
        return 0.0; 
    return node->eval(); 
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

inline std::shared_ptr<iNode> newAssign(std::shared_ptr<iNode> var, std::shared_ptr<iNode> expr) {
    return std::make_shared<assignNode>(var, expr);
}

