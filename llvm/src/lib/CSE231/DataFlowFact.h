

class DataflowFact {
public:
    const char * getVariableName();

protected:
    const char * _variableName;
};

class ConstantDataflowFact : DataflowFact {
public:
    int getValue();

private:
    int _value;
};
