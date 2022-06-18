import InterceptCommon;
import <cstdint>;
import <span>;

/*
    sealed class ExampleClass
    {
        static proto float TestFunction(string arg);
        static proto float GetMeAVector(out vector arg);
        static proto external void TestFunc(out vector arg); // external gets instance as first argument and can access instance variables

        int testIntMemberVariable = 5;
        string testStringMember = "stringtest";
        float testFloatArray[3] = {1,2,3};
    };
 */
class ExampleClass : public ScriptClassBaseSimple<"ExampleClass"> {
public:
    // static proto float TestFunction(string arg);
    static void TestFunction(FunctionArgumentsHandler& args, FunctionResultHandler& result) {
        //if (!args.verifyType<std::string_view>(0))
        //    __debugbreak(); // wrong type inside script declaration


        auto myString = args.GetAs<std::string_view>(0);

        result.SetAs<float>(133.7f);
    }

    // static proto void GetMeAVector(out vector arg);
    static void GetMeAVector(FunctionArgumentsHandler& args, FunctionResultHandler& result) {

        // set via out var
        args.SetAs<Vector3>(0, Vector3{1, 2, 3});
    }
    
    // static proto external void TestFunc(out vector arg);
    static void TestFunc(FunctionArgumentsHandler& args, FunctionResultHandler& result) {

        // For "external" functions, first argument is always pointer to a class instance which holds member variables
        auto classInstance = args.GetAs<ClassInstance*>(0);

        auto instanceName = classInstance->GetClassName();
        auto instanceType = classInstance->GetClassType().getType();

        auto testIntMemberVariable = classInstance->GetVariable("testIntMemberVariable")->GetAs<int>(); 
        auto testStringMember = classInstance->GetVariable("testStringMember")->GetAs<std::string_view>();
        auto testFloatArray = classInstance->GetVariable("testFloatArray")->GetAs<std::span<float>>();

        // set via out var
        args.SetAs<Vector3>(1, Vector3{(float)testIntMemberVariable, 2, 3});
    }

    void DoSetup(ScriptClassBaseSimple::RegisterFuncHandler registerFunction) override {
        // We need to assign Enscript function name, to our function implementation here

        registerFunction("TestFunction", &TestFunction);
        registerFunction("GetMeAVector", &GetMeAVector);
        registerFunction("TestFunc", &TestFunc);
    }
};

ExampleClass GExampleClass; // Register the class, it needs to be a global that never gets deleted
