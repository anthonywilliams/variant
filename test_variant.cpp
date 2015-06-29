#include "variant"
#include <assert.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <array>
#include <tuple>

namespace se=std::experimental;

void initial_is_empty(){
    se::variant<int> v;
    assert(v.empty());
}

void empty_index_is_neg_one(){
    se::variant<int> v;
    assert(v.index()==-1);
}

void get_empty_t_works_for_empty(){
    se::variant<int> v;

    se::empty_t& e1=se::get<se::empty_t>(v);
    se::empty_t& e2=se::get<se::empty_t>(v);
    assert(&e1==&e2);
}

void can_construct_first_type(){
    se::variant<int> v(42);
    assert(v.index()==0);
}

void can_get_value_of_first_type(){
    se::variant<int> v(42);
    int& i=se::get<int>(v);
    assert(i==42);
}

void can_construct_second_type(){
    se::variant<int,std::string> v(std::string("hello"));
    assert(v.index()==1);
    std::string& s=se::get<std::string>(v);
    assert(s=="hello");
}

void can_move_variant(){
    se::variant<int,std::string> v(std::string("hello"));
    se::variant<int,std::string> v2(std::move(v));
    assert(v2.index()==1);
    assert(v.index()==-1);
    std::string& s=se::get<std::string>(v2);
    assert(s=="hello");
}

void can_copy_variant(){
    se::variant<int,std::string> v(std::string("hello"));
    se::variant<int,std::string> v2(v);
    assert(v2.index()==1);
    assert(v.index()==1);
    std::string& s=se::get<std::string>(v);
    assert(s=="hello");
    std::string& s2=se::get<std::string>(v2);
    assert(s2=="hello");
    assert(&s!=&s2);
}

void can_copy_const_variant(){
    se::variant<int,std::string> const v(std::string("hello"));
    se::variant<int,std::string> v2(v);
    assert(v2.index()==1);
    assert(v.index()==1);
    std::string const& s=se::get<std::string>(v);
    assert(s=="hello");
    std::string& s2=se::get<std::string>(v2);
    assert(s2=="hello");
    assert(&s!=&s2);
}

void construction_from_lvalue(){
    std::vector<int> vec(42);
    se::variant<std::vector<int>> v(vec);
    assert(vec.size()==42);
    assert(v.index()==0);
    std::vector<int>& vec2=se::get<std::vector<int>>(v);
    assert(&vec2!=&vec);
    assert(vec2.size()==42);
}

void construction_from_const_lvalue(){
    std::vector<int> const vec(42);
    se::variant<std::vector<int>> v(vec);
    assert(vec.size()==42);
    assert(v.index()==0);
    std::vector<int>& vec2=se::get<std::vector<int>>(v);
    assert(&vec2!=&vec);
    assert(vec2.size()==42);
}

void move_construction_with_move_only_types(){
    std::unique_ptr<int> ui(new int(42));
    se::variant<std::unique_ptr<int>> v(std::move(ui));
    assert(v.index()==0);
    std::unique_ptr<int>& p2=se::get<std::unique_ptr<int>>(v);
    assert(p2);
    assert(*p2==42);
    
    se::variant<std::unique_ptr<int>> v2(std::move(v));
    assert(v.index()==-1);
    assert(v2.index()==0);
    std::unique_ptr<int>& p3=se::get<std::unique_ptr<int>>(v2);
    assert(p3);
    assert(*p3==42);
}

struct CopyCounter{
    unsigned move_construct=0;
    unsigned copy_construct=0;
    unsigned move_assign=0;
    unsigned copy_assign=0;

    CopyCounter(){}
    CopyCounter(const CopyCounter& rhs):
        move_construct(rhs.move_construct),
        copy_construct(rhs.copy_construct+1),
        move_assign(rhs.move_assign),
        copy_assign(rhs.copy_assign)
    {}
    CopyCounter(CopyCounter&& rhs):
        move_construct(rhs.move_construct+1),
        copy_construct(rhs.copy_construct),
        move_assign(rhs.move_assign),
        copy_assign(rhs.copy_assign)
    {}
    CopyCounter& operator=(const CopyCounter& rhs){
        move_construct=rhs.move_construct;
        copy_construct=rhs.copy_construct;
        move_assign=rhs.move_assign;
        copy_assign=rhs.copy_assign+1;
        return *this;
    }
    CopyCounter& operator=(CopyCounter&& rhs){
        move_construct=rhs.move_construct;
        copy_construct=rhs.copy_construct;
        move_assign=rhs.move_assign+1;
        copy_assign=rhs.copy_assign;
        return *this;
    }
};

void copy_assignment_same_type(){
    CopyCounter cc;
    se::variant<CopyCounter> v(cc);
    assert(v.index()==0);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    se::variant<CopyCounter> v2(cc);
    v2=v;
    assert(v2.index()==0);
    assert(se::get<CopyCounter>(v2).copy_construct==1);
    assert(se::get<CopyCounter>(v2).move_construct==0);
    assert(se::get<CopyCounter>(v2).copy_assign==1);
    assert(se::get<CopyCounter>(v2).move_assign==0);
}

void copy_assignment_to_empty(){
    CopyCounter cc;
    se::variant<CopyCounter> v(cc);
    assert(v.index()==0);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    se::variant<CopyCounter> v2;
    v2=v;
    assert(v.index()==0);
    assert(v2.index()==0);
    assert(se::get<CopyCounter>(v2).copy_construct==2);
    assert(se::get<CopyCounter>(v2).move_construct==0);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==0);
}

struct InstanceCounter{
    static unsigned instances;

    InstanceCounter(){
        ++instances;
    }
    InstanceCounter(InstanceCounter const& rhs){
        ++instances;
    }
    ~InstanceCounter(){
        --instances;
    }
};

unsigned InstanceCounter::instances=0;
    
void copy_assignment_of_diff_types_destroys_old(){
    se::variant<InstanceCounter,int> v;
    assert(InstanceCounter::instances==0);
    v=se::variant<InstanceCounter,int>(InstanceCounter());
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    se::variant<InstanceCounter,int> v2(42);
    v=v2;
    assert(v.index()==1);
    assert(v2.index()==1);
    assert(se::get<int>(v2)==42);
    assert(se::get<int>(v)==42);
    assert(InstanceCounter::instances==0);
}

void copy_assignment_from_empty(){
    se::variant<InstanceCounter,int> v=InstanceCounter();
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    se::variant<InstanceCounter,int> v2;
    v=v2;
    assert(v.index()==-1);
    assert(v2.index()==-1);
    assert(InstanceCounter::instances==0);
}

struct CopyError{};

struct ThrowingCopy{
    ThrowingCopy(){}
    ThrowingCopy(ThrowingCopy const&){
        throw CopyError();
    }
    ThrowingCopy(ThrowingCopy&&){}
    ThrowingCopy operator=(ThrowingCopy const&){
        throw CopyError();
    }
};

void throwing_copy_assign_leaves_target_empty(){
    se::variant<InstanceCounter,ThrowingCopy> v=InstanceCounter();
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    se::variant<InstanceCounter,ThrowingCopy> v2{ThrowingCopy()};
    try{
        v=v2;
        assert(!"Exception should be thrown");
    }
    catch(CopyError&){
    }
    assert(v.index()==-1);
    assert(InstanceCounter::instances==0);
}

void move_assignment_to_empty(){
    CopyCounter cc;
    se::variant<CopyCounter> v(cc);
    assert(v.index()==0);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    se::variant<CopyCounter> v2;
    v2=std::move(v);
    assert(v.index()==-1);
    assert(v2.index()==0);
    assert(se::get<CopyCounter>(v2).copy_construct==1);
    assert(se::get<CopyCounter>(v2).move_construct==1);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==0);
}

void move_assignment_same_type(){
    CopyCounter cc;
    se::variant<CopyCounter> v(cc);
    assert(v.index()==0);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    se::variant<CopyCounter> v2(std::move(cc));
    v2=std::move(v);
    assert(v.index()==-1);
    assert(v2.index()==0);
    assert(se::get<CopyCounter>(v2).copy_construct==1);
    assert(se::get<CopyCounter>(v2).move_construct==0);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==1);
}

void move_assignment_of_diff_types_destroys_old(){
    se::variant<InstanceCounter,CopyCounter> v;
    assert(InstanceCounter::instances==0);
    v=se::variant<InstanceCounter,CopyCounter>(InstanceCounter());
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    se::variant<InstanceCounter,CopyCounter> v2{CopyCounter()};
    v=std::move(v2);
    assert(v.index()==1);
    assert(v2.index()==-1);
    assert(InstanceCounter::instances==0);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==2);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void move_assignment_from_empty(){
    se::variant<InstanceCounter,int> v=InstanceCounter();
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    se::variant<InstanceCounter,int> v2;
    v=std::move(v2);
    assert(v.index()==-1);
    assert(v2.index()==-1);
    assert(InstanceCounter::instances==0);
}

void emplace_construction_by_type(){
    const char* const msg="hello";
    se::variant<int,char const*,std::string> v(
        se::emplaced_type_t<std::string>(),msg);
    assert(v.index()==2);
    assert(se::get<2>(v)==msg);
}

void emplace_construction_by_index(){
    const char* const msg="hello";
    se::variant<int,char const*,std::string> v(
        se::emplaced_index_t<2>(),msg);
    assert(v.index()==2);
    assert(se::get<2>(v)==msg);
}

void holds_alternative_for_empty_variant(){
    se::variant<int,double> v;
    assert(!se::holds_alternative<int>(v));
    assert(!se::holds_alternative<double>(v));
    assert(se::holds_alternative<se::empty_t>(v));
}

void holds_alternative_for_non_empty_variant(){
    se::variant<int,double> v(2.3);
    assert(!se::holds_alternative<int>(v));
    assert(se::holds_alternative<double>(v));
    assert(!se::holds_alternative<se::empty_t>(v));
}

void assignment_from_value_to_empty(){
    CopyCounter cc;
    se::variant<int,CopyCounter> v;
    v=cc;
    assert(v.index()==1);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void assignment_from_value_to_same_type(){
    CopyCounter cc;
    se::variant<int,CopyCounter> v(cc);
    v=cc;
    assert(v.index()==1);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==1);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void assignment_from_value_of_diff_types_destroys_old(){
    se::variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    v=CopyCounter();
    assert(v.index()==1);
    assert(InstanceCounter::instances==0);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==1);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void emplace_from_value_to_empty(){
    const char* const msg="hello";
    se::variant<int,char const*,std::string> v;
    v.emplace<std::string>(msg);
    assert(v.index()==2);
    assert(se::get<2>(v)==msg);
}

void emplace_from_value_to_same_type(){
    CopyCounter cc;
    se::variant<int,CopyCounter> v(cc);
    v.emplace<CopyCounter>();
    assert(v.index()==1);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void emplace_from_value_of_diff_types_destroys_old(){
    se::variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    v.emplace<CopyCounter>();
    assert(v.index()==1);
    assert(InstanceCounter::instances==0);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void emplace_by_index_to_empty(){
    const char* const msg="hello";
    se::variant<int,char const*,std::string> v;
    v.emplace<2>(msg);
    assert(v.index()==2);
    assert(se::get<2>(v)==msg);
}

void emplace_by_index_to_same_type(){
    CopyCounter cc;
    se::variant<int,CopyCounter> v(cc);
    v.emplace<1>();
    assert(v.index()==1);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void emplace_by_index_of_diff_types_destroys_old(){
    se::variant<InstanceCounter,CopyCounter> v{InstanceCounter()};
    assert(v.index()==0);
    assert(InstanceCounter::instances==1);
    v.emplace<1>();
    assert(v.index()==1);
    assert(InstanceCounter::instances==0);
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);
}

void swap_same_type(){
    se::variant<int,CopyCounter> v{CopyCounter()};
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==1);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    CopyCounter cc;
    se::variant<int,CopyCounter> v2{cc};
    assert(se::get<CopyCounter>(v2).copy_construct==1);
    assert(se::get<CopyCounter>(v2).move_construct==0);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==0);
    v.swap(v2);

    assert(v.index()==1);
    assert(v2.index()==1);
    assert(se::get<CopyCounter>(v).copy_construct==1);
    assert(se::get<CopyCounter>(v).move_construct==0);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==1);
    assert(se::get<CopyCounter>(v2).copy_construct==0);
    assert(se::get<CopyCounter>(v2).move_construct==2);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==1);
}

void swap_different_types(){
    se::variant<int,CopyCounter> v{CopyCounter()};
    assert(se::get<CopyCounter>(v).copy_construct==0);
    assert(se::get<CopyCounter>(v).move_construct==1);
    assert(se::get<CopyCounter>(v).copy_assign==0);
    assert(se::get<CopyCounter>(v).move_assign==0);

    se::variant<int,CopyCounter> v2{42};
    v.swap(v2);

    assert(v.index()==0);
    assert(v2.index()==1);
    assert(se::get<CopyCounter>(v2).copy_construct==0);
    assert(se::get<CopyCounter>(v2).move_construct==3);
    assert(se::get<CopyCounter>(v2).copy_assign==0);
    assert(se::get<CopyCounter>(v2).move_assign==0);
}

void assign_empty_to_empty(){
    se::variant<int> v1,v2;
    v1=v2;
    assert(v1.index()==-1);
    assert(v2.index()==-1);
}

void swap_empties(){
    se::variant<int> v1,v2;
    v1.swap(v2);
    assert(v1.index()==-1);
    assert(v2.index()==-1);
}

struct VisitorIS{
    int& i;
    std::string& s;

    void operator()(int arg){
        i=arg;
    }
    void operator()(std::string const& arg){
        s=arg;
    }
};

void visit(){
    se::variant<int,std::string> v(42);

    int i=0;
    std::string s;
    VisitorIS visitor{i,s};
    se::visit(visitor,v);
    assert(i==42);
    i=0;
    v=std::string("hello");
    se::visit(visitor,v);
    assert(s=="hello");
    try{
        se::variant<int,std::string> v2;
        se::visit(visitor,v2);
        assert(!"Visiting empty should throw");
    }
    catch(se::bad_variant_access){}
}

void reference_members(){
    int i=42;
    se::variant<int&> v(se::emplaced_index_t<0>(),i);

    assert(v.index()==0);
    assert(&se::get<int&>(v)==&i);
    assert(&se::get<0>(v)==&i);
}

void equality(){
    se::variant<int,double,std::string> v(42);
    se::variant<int,double,std::string> v2(4.2);
    se::variant<int,double,std::string> v3(std::string("42"));

    assert(v==v);
    assert(v!=v2);
    assert(v!=v3);
    assert(v2==v2);
    assert(v3==v3);
    se::variant<int,double,std::string> v4(v);
    assert(v==v4);
    v4=std::move(v2);
    assert(v4!=v2);
    assert(v2==v2);
    assert(v!=v2);
    v2=3;
    assert(v!=v2);
}

void less_than(){
    se::variant<int,double,std::string> v(42);
    se::variant<int,double,std::string> v2(4.2);
    se::variant<int,double,std::string> v3(std::string("42"));

    assert(!(v<v));
    assert(v>=v);
    assert(v<v2);
    assert(v<v3);
    assert(v2<v3);
    se::variant<int,double,std::string> v4(v);
    assert(!(v4<v));
    assert(!(v<v4));
    v4=std::move(v2);
    assert(v2<v4);
    assert(v2<v);
    assert(v2<v3);
    v2=99;
    assert(v<v2);
    assert(v2<v4);
    assert(v2<v3);
}

void constexpr_variant(){
    constexpr se::variant<int> v(42);
    constexpr int i=se::get<int>(v);
    assert(i==42);
    constexpr se::variant<int> v2(se::emplaced_index_t<0>(),42);
    constexpr int i2=se::get<int>(v2);
    assert(i2==42);
    constexpr se::variant<int> v3(se::emplaced_type_t<int>(),42);
    constexpr int i3=se::get<int>(v3);
    assert(i3==42);
    constexpr se::variant<int,double> v4(4.2);
    constexpr int i4=v4.index();
    assert(i4==1);
    constexpr bool b4=v4.empty();
    assert(!b4);
    constexpr se::variant<int,double> v5;
    constexpr int i5=v5.index();
    assert(i5==-1);
    constexpr bool b5=v5.empty();
    assert(b5);
    constexpr se::variant<> v6;
    constexpr bool b6=v6.empty();
    assert(b6);
}

struct VisitorISD{
    int& i;
    std::string& s;
    double& d;
    int& i2;
    
    void operator()(int arg,double d_){
        i=arg;
        d=d_;
    }
    void operator()(std::string const& arg,double d_){
        s=arg;
        d=d_;
    }
    void operator()(int arg,int i2_){
        i=arg;
        i2=i2_;
    }
    void operator()(std::string const& arg,int i2_){
        s=arg;
        i2=i2_;
    }
    void operator()(se::empty_t arg,double d_){
        d=d_;
    }
    void operator()(se::empty_t arg,int i2_){
        i2=i2_;
    }
    void operator()(int arg,se::empty_t){
        i=arg;
    }
    void operator()(std::string const& arg,se::empty_t){
        s=arg;
    }
    void operator()(se::empty_t,se::empty_t){
    }
};

void multivisitor(){
    se::variant<int,char,std::string> v(42);
    se::variant<double,int> v2(4.2);

    int i=0;
    std::string s;
    double d=0;
    int i2=0;
    VisitorISD visitor{i,s,d,i2};
    se::visit(visitor,v,v2);
    assert(i==42);
    assert(s=="");
    assert(d==4.2);
    assert(i2==0);
    i=0;
    d=0;
    v=std::string("hello");
    assert(v.index()==2);
    v2=37;
    se::visit(visitor,v,v2);
    assert(i==0);
    assert(s=="hello");
    assert(d==0);
    assert(i2==37);

    se::variant<double,int> v3;
    try{
        se::visit(visitor,v,v3);
        assert(!"Visiting empty should throw");
    }
    catch(se::bad_variant_access){}
}

void sizes(){
    std::cout<<"empty_t:"<<sizeof(se::empty_t)<<std::endl;
    std::cout<<"variant<>:"<<sizeof(se::variant<>)<<std::endl;
    std::cout<<"variant<char>:"<<sizeof(se::variant<char>)<<std::endl;
    std::cout<<"variant<char,int>:"<<sizeof(se::variant<char,int>)<<std::endl;
    std::cout<<"int:"<<sizeof(int)<<std::endl;
    std::cout<<"variant<char,double>:"<<sizeof(se::variant<char,double>)<<std::endl;
    std::cout<<"double:"<<sizeof(double)<<std::endl;
    std::cout<<"variant<char,std::string>:"<<sizeof(se::variant<char,std::string>)<<std::endl;
    std::cout<<"std::string:"<<sizeof(std::string)<<std::endl;
    std::cout<<"variant<char,std::pair<int,int>>:"<<sizeof(se::variant<char,std::pair<int,int>>)<<std::endl;
    std::cout<<"std::pair<int,int>:"<<sizeof(std::pair<int,int>)<<std::endl;
    std::cout<<"variant<char,std::pair<char,char>>:"<<sizeof(se::variant<char,std::pair<char,char>>)<<std::endl;
    std::cout<<"std::pair<char,char>:"<<sizeof(std::pair<char,char>)<<std::endl;
std::cout<<"variant<char,std::pair<double,char>>:"<<sizeof(se::variant<char,std::pair<double,char>>)<<std::endl;
    std::cout<<"std::pair<double,char>:"<<sizeof(std::pair<double,char>)<<std::endl;
    std::cout<<"variant<double,std::tuple<int,int,int>>:"<<sizeof(se::variant<double,std::tuple<int,int,int>>)<<std::endl;
    std::cout<<"std::tuple<int,int,int>:"<<sizeof(std::tuple<int,int,int>)<<std::endl;
}

void duplicate_types(){
    se::variant<int,int> v(42);
    assert(se::get<int>(v)==42);
    assert(v.index()==0);
    assert(se::get<0>(v)==42);

    se::variant<int,int> v2(se::emplaced_index_t<1>(),42);
    assert(v2.index()==1);
    assert(se::get<1>(v2)==42);
    assert(se::get<int>(v2)==42);
}
struct NonMovable{
    int i;
    NonMovable():i(42){}
    NonMovable(NonMovable&&)=delete;
    NonMovable& operator=(NonMovable&&)=delete;
};


void non_movable_types(){
    se::variant<NonMovable> v{se::emplaced_index_t<0>()};
    assert(se::get<0>(v).i==42);
    se::get<0>(v).i=37;
    v.emplace<NonMovable>();
    assert(se::get<0>(v).i==42);
}

void can_emplace_empty(){
    se::variant<int> v(42);
    v.emplace<se::empty_t>();
    assert(v.empty());
    se::variant<int,std::string> v2(42);
    v2.emplace<se::empty_t>();
    assert(v2.empty());
}

void can_emplace_empty_by_index(){
    se::variant<int> v(42);
    v.emplace<-1>();
    assert(v.empty());
    se::variant<int,std::string> v2(42);
    v2.emplace<-1>();
    assert(v2.empty());
}

void direct_init_reference_member(){
    int i=42;
    se::variant<int&> v(i);
    assert(&se::get<int&>(v)==&i);
}

void reference_types_preferred_for_lvalue(){
    int i=42;
    se::variant<int,int&> v(i);
    assert(v.index()==1);

    se::variant<int> v2(42);
    assert(v2.index()==0);
}

void construction_with_conversion(){
    se::variant<int,std::string> v("hello");
    assert(v.index()==1);
    assert(se::get<1>(v)=="hello");
}

void assignment_with_conversion(){
    se::variant<int,std::string> v;
    v="hello";
    assert(v.index()==1);
    assert(se::get<1>(v)=="hello");
}

int main(){
    initial_is_empty();
    empty_index_is_neg_one();
    get_empty_t_works_for_empty();
    can_construct_first_type();
    can_get_value_of_first_type();
    can_construct_second_type();
    can_move_variant();
    can_copy_variant();
    can_copy_const_variant();
    construction_from_lvalue();
    construction_from_const_lvalue();
    move_construction_with_move_only_types();
    copy_assignment_same_type();
    copy_assignment_to_empty();
    copy_assignment_of_diff_types_destroys_old();
    copy_assignment_from_empty();
    throwing_copy_assign_leaves_target_empty();
    move_assignment_to_empty();
    move_assignment_same_type();
    move_assignment_of_diff_types_destroys_old();
    move_assignment_from_empty();
    emplace_construction_by_type();
    emplace_construction_by_index();
    holds_alternative_for_empty_variant();
    holds_alternative_for_non_empty_variant();
    assignment_from_value_to_empty();
    assignment_from_value_to_same_type();
    assignment_from_value_of_diff_types_destroys_old();
    emplace_from_value_to_empty();
    emplace_from_value_to_same_type();
    emplace_from_value_of_diff_types_destroys_old();
    emplace_by_index_to_empty();
    emplace_by_index_to_same_type();
    emplace_by_index_of_diff_types_destroys_old();
    swap_same_type();
    swap_different_types();
    assign_empty_to_empty();
    swap_empties();
    visit();
    reference_members();
    equality();
    less_than();
    constexpr_variant();
    multivisitor();
    sizes();
    duplicate_types();
    non_movable_types();
    can_emplace_empty();
    can_emplace_empty_by_index();
    direct_init_reference_member();
    reference_types_preferred_for_lvalue();
    construction_with_conversion();
    assignment_with_conversion();
}
