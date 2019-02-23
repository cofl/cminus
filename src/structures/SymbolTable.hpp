#ifndef SymbolTable_HPP
    #define SymbolTable_HPP

#include <vector>
#include <memory>
namespace Cminus { namespace Structures
{
    using namespace std;
    class SymbolTableFrame;

    class SymbolTable
    {
        private:
            vector<shared_ptr<SymbolTableFrame>> _frames;
            shared_ptr<SymbolTableFrame> _globalFrame;
        public:
            SymbolTable();
            ~SymbolTable();

        #pragma region Methods
            void EnterScope();
            void ExitScope();
        #pragma endregion Methods
    };

    class SymbolTableFrame
    {
        private:
        public:
            SymbolTableFrame();
            ~SymbolTableFrame();
    };
}}

#endif
