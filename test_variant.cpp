#include "variant"
#include <assert.h>

void initial_is_empty(){
    std::experimental::variant<int> v;
    assert(v.empty());
}

void empty_index_is_neg_one(){
    std::experimental::variant<int> v;
    assert(v.index()==-1);
}

int main(){
    initial_is_empty();
    empty_index_is_neg_one();
    
}
