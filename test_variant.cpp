#include "variant"
#include <assert.h>

void initial_is_empty(){
    std::experimental::variant<int> v;
    assert(v.empty());
}

int main(){
    initial_is_empty();
    
}
