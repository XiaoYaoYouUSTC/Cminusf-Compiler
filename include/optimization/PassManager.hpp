#ifndef PASSMANAGER_HPP
#define PASSMANAGER_HPP



#include "Module.h"
#include <vector>
#include <memory>
// using PassPtr = ;
// using PassPtrList=;
class Pass{
public:
    Pass(Module* m) : m_(m){
    }

    virtual void run()=0;

protected:
    
    Module* m_;
};

class PassManager{
    public:
        PassManager(Module* m) : m_(m){}
        template<typename PassType> void add_pass(bool print_ir=false){
            passes_.push_back(std::pair<Pass*,bool>(new PassType(m_),print_ir));
        }
        void run(){
            for(auto pass : passes_){
                pass.first->run();
                if(pass.second){
                    std::cout<<m_->print();
                }
            }
        }


    private:
        std::vector<std::pair<Pass*,bool>> passes_;
        Module* m_;

};

#endif