#include "parser.tab.h"
#include "scanner.h"
#include "ast.h"
#include "codegen.h"

extern int ch;
extern int parserror;
int codegenerror = 0;
using namespace llvm;
using namespace llvm::sys;
LLVMContext TheContext;
IRBuilder<> Builder(TheContext);
std::map<const char*, int> BinopPrecedence;
std::unique_ptr<Module> TheModule;
std::map<std::string, AllocaInst *> NamedValues;
void yyerror(char *errmsg)
{
    parserror += 1;
    fprintf(stderr, "%s (%d, %d): %s\n", errmsg, yylineno, ch, yytext);
}

static AllocaInst *CreateEntryBlockAlloca(Function *TheFunction, const char *VarName)
{
    IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
                       TheFunction->getEntryBlock().begin());
    return TmpB.CreateAlloca(Type::getInt32Ty(TheContext), nullptr, VarName);
}

Value *LogErrorV(const char *Str)
{
    codegenerror += 1;
    std::cout << Str << std::endl;
    return nullptr;
}

Value *codegen(PNode *root)
{
    Value *L, *R;
    printf("%d %s\n", root->Type, root->lexeme);
    switch(root->Type)
    {
    case TyNumber:
        return ConstantInt::get(TheContext, APInt(32, atoi(root->lexeme)));
    case TyIdentifier:
    {
        Value *V = NamedValues[root->lexeme];
        if(!V)
            return LogErrorV("Unknown variable name");
        return Builder.CreateLoad(V, root->lexeme);
    }
    case TyString:
    {
        llvm::Constant *strFormatStr = llvm::ConstantDataArray::getString(TheContext, root->lexeme);
        auto *strFormatStrLoc = llvm::cast<llvm::GlobalVariable>(TheModule->getOrInsertGlobal(root->lexeme, strFormatStr->getType()));
        strFormatStrLoc->setInitializer(strFormatStr);
        return strFormatStr;
    }
    case TyPlus:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateAdd(L, R, "addtmp");
    case TyMinus:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateSub(L, R, "subtmp");
    case TyMul:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateMul(L, R, "multmp");
    case TyDivision:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateUDiv(L, R, "divtmp");
    case TyOver:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpSGT(L, R, "overtmp");
    case TyEql:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpEQ(L, R, "eqtmp");
    case TyNotEql:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpNE(L, R, "noteqtmp");
    case TyLess:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpSLT(L, R, "lesstmp");;
    case OverEq:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpSGE(L, R, "overeqltmp");
    case LessEq:
        if(root->son1 != NULL)
            L = codegen(root->son1);
        if(root->son2 != NULL)
            R = codegen(root->son2);

        if(!L || !R)
            return nullptr;
        return Builder.CreateICmpSLE(L, R, "lesseqltmp");
    case TyInt:
    {
        AllocaInst *OldBindings;
        if(!root->son1)
            return nullptr;
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        AllocaInst *Alloca = CreateEntryBlockAlloca(TheFunction, root->son1->lexeme);
        Value *InitVal;
        if(root->son2 == NULL)
            InitVal = ConstantInt::get(TheContext, APInt(32, 0));
        else
            InitVal = codegen(root->son2);
        OldBindings = NamedValues[root->son1->lexeme];
        Builder.CreateStore(InitVal, Alloca);
        NamedValues[root->son1->lexeme] = Alloca;
        //printf("%s", root->son1->lexeme);
        if(root->son3)
        {
            R = codegen(root->son3);
            NamedValues[root->son1->lexeme] = OldBindings;
            return R;
        }
        NamedValues[root->son1->lexeme] = OldBindings;
        return InitVal;
    }
    case TySet:
    {
        if(root->son2 != NULL)
            R = codegen(root->son2);
        if(!R)
            return nullptr;
        if(root->son1->Type != TyIdentifier)
            return LogErrorV("destination of '=' must be a variable");
        Value *Variable = NamedValues[root->son1->lexeme];
        if(!Variable)
            return LogErrorV("Unknown variable name");
        Builder.CreateStore(R, Variable);
        return R;
    }
    case TyPrint:
    {
        Function *TheFunction = TheModule->getFunction("printf");
        llvm::Value *formatStrGlobal;
        Value * Arg;
        Arg = codegen(root->son1);
        if(root->son1->Type == TyNumber || root->son1->Type == TyIdentifier)
        {
            formatStrGlobal = llvm::cast<llvm::Value>(TheModule->getGlobalVariable("intFormatStr"));
        }
        if(root->son1->Type == TyString)
        {
            formatStrGlobal = llvm::cast<llvm::Value>(TheModule->getGlobalVariable("strFormatStr"));
            Arg = llvm::cast<llvm::Value>(TheModule->getGlobalVariable(root->son1->lexeme));
        }
        llvm::Value *formatStr = Builder.CreatePointerCast(formatStrGlobal, TheFunction->arg_begin()->getType(), "formatstr");
        return Builder.CreateCall(TheFunction, {formatStr, Arg});
    }
    case TyIf:
    case TyElse:
        {
            L = codegen(root->son1);
            if(!L)
               return nullptr;
            Function *TheFunction = Builder.GetInsertBlock()->getParent();
            BasicBlock *ThenBB = BasicBlock::Create(TheContext, "then", TheFunction);
            BasicBlock *ElseBB = BasicBlock::Create(TheContext, "else");
            BasicBlock *MergeBB = BasicBlock::Create(TheContext, "ifcont");
            Builder.CreateCondBr(L, ThenBB, ElseBB);

            Builder.SetInsertPoint(ThenBB);
            R = codegen(root->son2);
            if(!R)
                return nullptr;
            Builder.CreateBr(MergeBB);
            ThenBB = Builder.GetInsertBlock();
            TheFunction->getBasicBlockList().push_back(ElseBB);
            Builder.SetInsertPoint(ElseBB);
            Value *ElseV;
            if(root->son3)
                ElseV = codegen(root->son3);
            else
                ElseV = ConstantInt::get(TheContext, APInt(32, 0));
            Builder.CreateBr(MergeBB);
            ElseBB = Builder.GetInsertBlock();
            TheFunction->getBasicBlockList().push_back(MergeBB);
            Builder.SetInsertPoint(MergeBB);
            PHINode *PN = Builder.CreatePHI(Type::getInt32Ty(TheContext), 2, "thentmp");
            PN->addIncoming(R, ThenBB);
            PN->addIncoming(ElseV, ElseBB);
            return PN;
        }
    case TySeq:
    case TyExpr:
    case TyOps:
    {
        L = codegen(root->son1);
        if(root->son2 != NULL)
        {
            R = codegen(root->son2);
            return R;
        }
        return L;
    }
    case TyWhile:
    {
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
        Builder.CreateBr(LoopBB);
        Builder.SetInsertPoint(LoopBB);
        R = codegen(root->son2);
        L = codegen(root->son1);
        if(!L)
            return nullptr;
        BasicBlock *AfterBB = BasicBlock::Create(TheContext, "afterloop", TheFunction);
        Builder.CreateCondBr(L, LoopBB, AfterBB);
        Builder.SetInsertPoint(AfterBB);
        return Constant::getNullValue(Type::getInt32Ty(TheContext));
    }
    case TyDo:
    {
        R = codegen(root->son1);
        Function *TheFunction = Builder.GetInsertBlock()->getParent();
        BasicBlock *LoopBB = BasicBlock::Create(TheContext, "loop", TheFunction);
        Builder.CreateBr(LoopBB);
        Builder.SetInsertPoint(LoopBB);
        R = codegen(root->son1);
        L = codegen(root->son2);
        if(!L)
            return nullptr;
        BasicBlock *AfterBB = BasicBlock::Create(TheContext, "afterloop", TheFunction);
        Builder.CreateCondBr(L, LoopBB, AfterBB);
        Builder.SetInsertPoint(AfterBB);
        return Constant::getNullValue(Type::getInt32Ty(TheContext));
    }
    default:
        return LogErrorV("Unknown operations");
    }
}

void MainLoop(PNode *root)
{
    if(root)
    {
        Function *IR;
        llvm::FunctionType *printf = llvm::TypeBuilder<int(char *, ...), false>::get(TheContext);
        auto *printfFunc = llvm::cast<llvm::Function>(TheModule->getOrInsertFunction("printf", printf));
        llvm::Constant *intFormatStr = llvm::ConstantDataArray::getString(TheContext, "%d\n");
        auto *intFormatStrLoc = llvm::cast<llvm::GlobalVariable>(TheModule->getOrInsertGlobal("intFormatStr", intFormatStr->getType()));
        llvm::Constant *strFormatStr = llvm::ConstantDataArray::getString(TheContext, "%s\n");
        auto *strFormatStrLoc = llvm::cast<llvm::GlobalVariable>(TheModule->getOrInsertGlobal("strFormatStr", strFormatStr->getType()));
        printfFunc->addAttribute(1u, llvm::Attribute::NoAlias);
        printfFunc->addAttribute(1u, llvm::Attribute::NoCapture);
        strFormatStrLoc->setInitializer(strFormatStr);
        intFormatStrLoc->setInitializer(intFormatStr);
        if(root->Type == TyMain)
        {
            FunctionType *FT = FunctionType::get(Type::getInt32Ty(TheContext), false);
            //std::vector<Value *> ArgsV;
            Function::Create(FT, llvm::Function::ExternalLinkage, "main", TheModule.get());
            IR = TheModule->getFunction("main");
            //return Builder.CreateCall(Callee, NULL, "maintmp");
            BasicBlock *BB = BasicBlock::Create(TheContext, "main", IR);
            Builder.SetInsertPoint(BB);
            Value *RetVal = codegen(root->son1);
            //printf("root %d", root->Type);
            Builder.CreateRet(RetVal);
            verifyFunction(*IR);  
        }else{
            return;
        }
        IR->print(errs());
    }
}

int main(int argc, char **argv)
{
    extern int ch;
    extern int parserror;
    extern PNode *root;
    extern const char* token_name(int t);
    int l, flag;
    if(argc < 3)
    {
        printf("\nNot enough arguments. Please specify filename and mode\n");
        return -1;
    }
    if((yyin = fopen(argv[1], "r")) == NULL)
    {
        printf("\nCannot open file %s.\n", argv[1]);
        return -1;
    }
    sscanf(argv[2], "%d", &flag);
    ch = 1;
    yylineno = 1;
    switch(flag){
    case 1:
        while((l = yylex()))
        {
            printf("found token %s (%d, %d): %s\n", token_name(l), yylineno, ch, yytext);
        }
        break;
    case 2:
    case 3:
        yyparse();
        if(parserror == 0)
        {
            if(flag == 2)
            {
                tree_print(root, 0);
                rapidjson::Document doc;
                toJSON(doc, root);
                rapidjson::StringBuffer buffer;
                // rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
                doc.Accept(writer);
                const std::string& str = buffer.GetString();
               // std::cout << "Serialized:" << std::endl;
               // std::cout << str << std::endl;
                std::ofstream out;
                out.open("./ast.json");
                if (out.is_open())
                {
                    out << str << std::endl;
                }
            }else{
                BinopPrecedence["<"] = 10;
                BinopPrecedence[">"] = 10;
                BinopPrecedence["=="] = 10;
                BinopPrecedence[">="] = 10;
                BinopPrecedence["<="] = 10;
                BinopPrecedence["!="] = 10;
                BinopPrecedence["+"] = 20;
                BinopPrecedence["-"] = 20;
                BinopPrecedence["*"] = 40;
                BinopPrecedence["/"] = 20;
                TheModule = llvm::make_unique<Module>("jit", TheContext);
                MainLoop(root);

                InitializeAllTargetInfos();
                InitializeAllTargets();
                InitializeAllTargets();
                InitializeAllTargetMCs();
                InitializeAllAsmParsers();
                InitializeAllAsmPrinters();

                auto TargetTriple = sys::getDefaultTargetTriple();
                TheModule->setTargetTriple(TargetTriple);
                std::string Error;
                auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);
                if(!Target)
                {
                    errs() << Error;
                    return -1;
                }
                auto CPU = "generic";
                auto Features = "";

                TargetOptions opt;
                auto RM = Optional<Reloc::Model>();
                auto TheTargetMachine =
                    Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

                TheModule->setDataLayout(TheTargetMachine->createDataLayout());

                auto Filename = "output.o";
                std::error_code EC;
                llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::F_None);

                if (EC) {
                    errs() << "Could not open file: " << EC.message();
                    return 1;
                }

                legacy::PassManager pass;
                auto FileType = TargetMachine::CGFT_ObjectFile;

                if (TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
                    errs() << "TheTargetMachine can't emit a file of this type";
                    return 1;
                }

                pass.run(*TheModule);
                dest.flush();

                outs() << "Wrote " << Filename << "\n";

            }
        }
        break;
    }
    fclose(yyin);
    return 0;
}