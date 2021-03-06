#ifndef MSL_GENERATOR_H
#define MSL_GENERATOR_H

#include "CodeWriter.h"
#include "HLSLTree.h"

namespace M4
{

class  HLSLTree;
struct HLSLFunction;
struct HLSLStruct;
    
/**
 * This class is used to generate MSL shaders.
 */
class MSLGenerator
{
public:
    enum Target
    {
        Target_VertexShader,
        Target_FragmentShader,
    };
    
    enum Flags
    {
        Flag_ConstShadowSampler = 1 << 0,
        Flag_PackMatrixRowMajor = 1 << 1,
        Flag_NoIndexAttribute   = 1 << 2,
    };

    struct Options
    {
        unsigned int flags;
        unsigned int bufferRegisterOffset;
        int (*attributeCallback)(const char* name, unsigned int index);
        bool treatHalfAsFloat;
        bool forceHalfPrecision;    // Not implemented.
        bool usePreciseFma;
        bool use16BitIntegers;

        Options()
        {
            flags = 0;
            bufferRegisterOffset = 0;
            attributeCallback = NULL;
            treatHalfAsFloat = true;
            forceHalfPrecision = false;
            usePreciseFma = false;
            use16BitIntegers = false;
        }
    };

    MSLGenerator();

    bool Generate(HLSLTree* tree, Target target, const char* entryName, const Options& options = Options());
    const char* GetResult() const;

private:
    
    // @@ Rename class argument. Add buffers & textures.
    struct ClassArgument
    {
        const char* name;
        HLSLType type;
        //const char* typeName;     // @@ Do we need more than the type name?
        const char* registerName;

        ClassArgument * nextArg;
        
        ClassArgument(const char* name, HLSLType type, const char * registerName) :
            name(name), type(type), registerName(registerName)
		{
			nextArg = NULL;
		}
    };

    void AddClassArgument(ClassArgument * arg);

    void Prepass(HLSLTree* tree, Target target, HLSLFunction* entryFunction);
    void CleanPrepass();
    
    void PrependDeclarations();
    
    void OutputStatements(int indent, HLSLStatement* statement);
    void OutputAttributes(int indent, HLSLAttribute* attribute);
    void OutputDeclaration(HLSLDeclaration* declaration);
    void OutputStruct(int indent, HLSLStruct* structure);
    void OutputBuffer(int indent, HLSLBuffer* buffer);
    void OutputFunction(int indent, HLSLFunction* function);
    void OutputExpression(HLSLExpression* expression, HLSLExpression* parentExpression);
    void OutputTypedExpression(const HLSLType& type, HLSLExpression* expression, HLSLExpression* parentExpression);
    bool NeedsCast(const HLSLType & target, const HLSLType & source);
    void OutputCast(const HLSLType& type);
    
    void OutputArguments(HLSLArgument* argument);
    void OutputDeclaration(const HLSLType& type, const char* name, HLSLExpression* assignment, bool isRef = false, bool isConst = false, int alignment = 0);
    void OutputDeclarationType(const HLSLType& type, bool isConst = false, bool isRef = false, int alignment = 0, bool isTypeCast = false);
    void OutputDeclarationBody(const HLSLType& type, const char* name, HLSLExpression* assignment, bool isRef = false);
    void OutputExpressionList(HLSLExpression* expression);
    void OutputExpressionList(const HLSLType& type, HLSLExpression* expression);
    void OutputExpressionList(HLSLArgument* argument, HLSLExpression* expression);
    
    void OutputFunctionCallStatement(int indent, HLSLFunctionCall* functionCall, HLSLDeclaration* assingmentExpression);
    void OutputFunctionCall(HLSLFunctionCall* functionCall, HLSLExpression * parentExpression);

    const char* TranslateInputSemantic(const char* semantic);
    const char* TranslateOutputSemantic(const char* semantic);

    const char* GetTypeName(const HLSLType& type, bool exactType);
    
    void Error(const char* format, ...);

private:

    CodeWriter      m_writer;

    HLSLTree*       m_tree;
    const char*     m_entryName;
    Target          m_target;
    Options         m_options;

    bool            m_error;

    ClassArgument * m_firstClassArgument;
    ClassArgument * m_lastClassArgument;
    
    HLSLFunction *  m_currentFunction;
};

} // M4

#endif
