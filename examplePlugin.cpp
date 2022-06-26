import InterceptCommon;
import <cstdint>;
import <span>;

/*
    sealed class ExampleClass
    {
        static proto float TestFunction(string arg);
        static proto float GetMeAVector(out vector arg);
        static proto external void TestFunc(out vector arg); // external gets instance as first argument and can access instance variables
        static proto string StringFunc(string inputString, out string arg);
        static proto void ArrayTest(notnull array<string> stringArray, notnull array<vector3> vectorArray, notnull array<float> floatArray);

        int testIntMemberVariable = 5;
        string testStringMember = "stringtest";
        float testFloatArray[3] = {1,2,3};


        // "InterceptReady" is a "magic" variable, when your plugin class DoSetup method gets executed, it will be set to true.
        // You can use this to detect if your plugin has actually been loaded.
        static bool InterceptReady = false;
        
    };
 */
class ExampleClass : public ScriptClassBaseSimple<"ExampleClass"> {
public:
    // static proto float TestFunction(string arg);
    static void TestFunction(FunctionArgumentsHandler& args, FunctionResultHandler& result) {
        //if (!args.VerifyType<std::string_view>(0))
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
        auto instanceType = classInstance->GetClassType().GetType();

        auto testIntMemberVariable = classInstance->GetVariable("testIntMemberVariable")->GetAs<int>(); 
        auto testStringMember = classInstance->GetVariable("testStringMember")->GetAs<std::string_view>();
        auto testFloatArray = classInstance->GetVariable("testFloatArray")->GetAs<std::span<float>>();

        // set via out var
        args.SetAs<Vector3>(0, Vector3{(float)testIntMemberVariable, 2, 3});
    }

    // static proto string StringFunc(string inputString, out string arg);
    static void StringFunc(FunctionArgumentsHandler& args, FunctionResultHandler& result) {

        // Strings can also be read as string_view, but currently this cannot be used in Set, that will be fixed in the future
        auto inputString = args.GetAs<std::string_view>(0);

        // Set a string via out var, this will copy the string into Enfusion's own memory
        args.SetAs<const char*>(1, "testabc");
        result.SetAs<const char*>(inputString.data());
    }

    // static proto void ArrayTest(notnull array<string> stringArray, notnull array<vector3> vectorArray, notnull array<float> floatArray);
    static void ArrayTest(FunctionArgumentsHandler& args, FunctionResultHandler& result) {

        // Arrays can currently only be read, via std::span. Writing to arrays is not possible for now.
        // Technically the Vector3/float/int/bool, all values that don't need memory allocation by themselves, could be in-place overwritten to change them in the array.
        // But element adding/removing is not supported yet.


        // This is not checking for null, thats why we have notnull in the function definition

        auto stringArray = args.GetAs<std::span<const char*>>(0);
        auto vectorArray = args.GetAs<std::span<Vector3>>(1);
        auto floatArray = args.GetAs<std::span<float>>(2);

        /*
            auto vectorArray = new array<vector>();
            points.Insert( Vector( 0, 0, 0) );
            points.Insert( Vector( 5, 0, 0) );
            points.Insert( Vector( 8, 3, 0) );
            points.Insert( Vector( 6, 1, 0) );
            autoptr array<string> stringArray = {"jedna", "dva", "tri"};
            autoptr array<float> floatArray = {0.1, 0.2, 0.3};
            DedmensSecondClass.ArrayTest(stringArray, vectorArray, floatArray);
        */
    }


    void DoSetup(ScriptClassBaseSimple::RegisterFuncHandler registerFunction) override {
        // We need to assign Enscript function name, to our function implementation here

        registerFunction("TestFunction", &TestFunction);
        registerFunction("GetMeAVector", &GetMeAVector);
        registerFunction("TestFunc", &TestFunc);
        registerFunction("StringFunc", &StringFunc);
        registerFunction("ArrayTest", &ArrayTest);
    }
};

ExampleClass GExampleClass; // Register the class, it needs to be a global that never gets deleted
