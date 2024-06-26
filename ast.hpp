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

class Visitor {
public:
    virtual int visit(const class scopeNode&)  = 0;
    virtual int visit(const class ifNode&)     = 0;
    virtual int visit(const class whileNode&)  = 0;
    virtual int visit(const class notNode&)    = 0;
    virtual int visit(const class minusNode&)  = 0;
    virtual int visit(const class arithNode&)  = 0;
    virtual int visit(const class predNode&)   = 0;
    virtual int visit(const class numNode&)    = 0;
    virtual int visit(const class varNode&)    = 0;
    virtual int visit(const class inputNode&)  = 0;
    virtual int visit(const class outputNode&) = 0;
    virtual int visit(const class assignNode&) = 0;
    virtual ~Visitor() {}
};

class evalVisitor: public Visitor {
public:
    int visit(const class scopeNode&)  override;
    int visit(const class whileNode&)  override;
    int visit(const class notNode&)    override;
    int visit(const class minusNode&)  override;
    int visit(const class arithNode&)  override;
    int visit(const class ifNode&)     override;
    int visit(const class predNode&)   override;
    int visit(const class numNode&)    override;
    int visit(const class varNode&)    override;
    int visit(const class inputNode&)  override;
    int visit(const class outputNode&) override;
    int visit(const class assignNode&) override;
};

class iNode {    
protected:
    std::unique_ptr<iNode> left_ = nullptr;
    std::unique_ptr<iNode> right_ = nullptr;
public:
    iNode() = default;
    iNode(std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) noexcept: 
        left_(std::move(left)), 
        right_(std::move(right)) {}
    
    virtual ~iNode() = default;

    virtual int eval(Visitor&) const = 0;
    virtual void dump(int indent = 0) const = 0;
    friend evalVisitor;
    // friend void dumpTree(std::unique_ptr<iNode> node, int indent = 0) {
    //     if (node == nullptr) return;   
    //     dumpTree(node->left_, indent + 4);
    //     node->dump(indent);
    //     dumpTree(node->right_, indent + 4);
    // }

};

class scopeNode: public iNode {
public:
    scopeNode(std::unique_ptr<iNode> right, std::unique_ptr<iNode> left) noexcept: 
        iNode(std::move(right), std::move(left)) {}
    
    int eval(Visitor& visitor) const override;

    void dump(int indent = 0) const override {

    }
};

class ifNode: public iNode {
   std::unique_ptr<iNode> expr_ = nullptr; 
public:
    ifNode(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> if_stmt, std::unique_ptr<iNode> else_stmt) noexcept: 
        iNode(std::move(if_stmt), std::move(else_stmt)), 
        expr_(std::move(expr)){}

    friend evalVisitor;

    int eval(Visitor& visitor) const override;

    void dump(int indent = 0) const override {

    }
    
};

class whileNode: public iNode {
public:
    whileNode(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> stmt) noexcept: 
        iNode(std::move(expr), std::move(stmt)) {}

    int eval(Visitor& visitor) const override;

    void dump(int indent = 0) const override {

    }
};

class notNode: public iNode {
public:
    notNode(std::unique_ptr<iNode> left) noexcept: 
        iNode(std::move(left), nullptr) {}

    int eval(Visitor& visitor) const override;

    void dump(int indent = 0) const override {

    }
};

class minusNode: public iNode {
public:
    minusNode(std::unique_ptr<iNode> left) noexcept: 
        iNode(std::move(left), nullptr) {}

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {

    }
};

class arithNode: public iNode { 
    arith_t op_;
    inline static const std::unordered_map<arith_t, std::function<int(int, int)>> operations_ = {
        {arith_t::PLUS,  {[](int lhs, int rhs) {return lhs + rhs;}}},
        {arith_t::MINUS, {[](int lhs, int rhs) {return lhs - rhs;}}},
        {arith_t::MULT,  {[](int lhs, int rhs) {return lhs * rhs;}}},
        {arith_t::DIV,   {[](int lhs, int rhs) {
            if (rhs == 0) {
                throw std::runtime_error("Division by zero"); 
            }
                return lhs / rhs;}}
        }
    };
    inline static const std::unordered_map<arith_t, std::function<void()>> dumped_arith_ = {
        {arith_t::PLUS,  {[]() {std::cout << "+" << std::endl;}}},
        {arith_t::MINUS, {[]() {std::cout << "-" << std::endl;}}},
        {arith_t::MULT,  {[]() {std::cout << "*" << std::endl;}}},
        {arith_t::DIV,   {[]() {std::cout << "/" << std::endl;}}}
    };
public:
    arithNode(arith_t op, std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) noexcept: 
        op_(op), 
        iNode(std::move(left), std::move(right)) {}

    friend evalVisitor;

    int eval(Visitor&) const override;

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
    predNode(pred_t op, std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) noexcept: 
        op_(op), 
        iNode(std::move(left), std::move(right)) {}

    friend evalVisitor;

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "OP: ";
        dumped_pred_.at(op_)();
    }
};

class numNode: public iNode { 
    int value_ = 0;
public:
    explicit numNode(int value) noexcept: 
        iNode(), value_(value){}

    friend evalVisitor;

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Num: " << value_ << std::endl;
    }
};

class varNode: public iNode { 
    std::string id_;
public:
    explicit varNode(const std::string& id): 
        id_(id), 
        iNode() 
    {
        var_store.emplace(id_, 0);
    }

    friend evalVisitor;

    std::string name() const {return id_;}    

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Var: " << id_ << std::endl;
    }
};

class inputNode: public iNode {
public:
    inputNode() noexcept: iNode(){}

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        
    }
};

class outputNode: public iNode {
    std::string id_;
public:
    outputNode(std::string& id) noexcept: 
        iNode(), 
        id_(id){}

    friend evalVisitor;

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        
    }
};

class assignNode: public iNode { 
public:
    explicit assignNode(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> var) noexcept: 
        iNode(std::move(expr), std::move(var)) {}

    int eval(Visitor&) const override;

    void dump(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "=" << std::endl;
    }
};

inline std::unique_ptr<iNode> newScope(std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) {
    return std::make_unique<scopeNode>(std::move(left), std::move(right));
}

inline std::unique_ptr<iNode> newIf(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> if_stmt, std::unique_ptr<iNode> else_stmt) {
    return std::make_unique<ifNode>(std::move(expr), std::move(if_stmt), std::move(else_stmt));
}

inline std::unique_ptr<iNode> newWhile(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> stmt) {
    return std::make_unique<whileNode>(std::move(expr), std::move(stmt));
}

inline std::unique_ptr<iNode> newNot(std::unique_ptr<iNode> expr) {
    return std::make_unique<notNode>(std::move(expr));
}

inline std::unique_ptr<iNode> newMinus(std::unique_ptr<iNode> expr) {
    return std::make_unique<minusNode>(std::move(expr));
}

inline std::unique_ptr<iNode> newArith(arith_t type, std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) {
    return std::make_unique<arithNode>(type, std::move(left), std::move(right));
}

inline std::unique_ptr<iNode> newPred(pred_t type, std::unique_ptr<iNode> left, std::unique_ptr<iNode> right) {
    return std::make_unique<predNode>(type, std::move(left), std::move(right));
}

inline std::unique_ptr<iNode> newNumber(int value) {
    return std::make_unique<numNode>(value);
}

inline std::unique_ptr<iNode> newInput() {
    return std::make_unique<inputNode>();
}

inline std::unique_ptr<iNode> newOutput(std::string& id) {
    return std::make_unique<outputNode>(id);
}

inline std::unique_ptr<iNode> newVar(std::string& id) {
    return std::make_unique<varNode>(id);
}

inline std::unique_ptr<iNode> newAssign(std::unique_ptr<iNode> expr, std::unique_ptr<iNode> var) {
    return std::make_unique<assignNode>(std::move(expr), std::move(var));
}

