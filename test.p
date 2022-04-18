// Inverts the given number if it is > 0 and divisible by 2
(i32 num) -> i32 invert_mod2
{
    if(num > 0 && num % 2 == 0)
    {
        return -num;
    }

    return num;
}

() -> i32 main
{
    i32 a0 = 4;
    dump(a0);
    i32 a_1 = a0;
    dump(invert(a_1));
    a_1 = a_1 + a0;
    dump(a_1);

    f32 _b = 1.5;
    dump(_b);

    str equation = "1 + 2 = 3!";

    return 1;
}
