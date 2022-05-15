struct ref_value_overloaded_struct
{
    int i = 1;
    int  get()&& { return i; } // call to this overload is rvalue
    int& get()& { return i; } // call to this overload is lvalue
};