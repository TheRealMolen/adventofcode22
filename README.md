# advent of code -- template c++ project

This is a template project to solve problems from https://adventofcode.com/ in C++ in **Visual Studio 2019** _(or later)_

It'll only run on Windows *(because of some fancy conio.dll stuff that makes the window sparkle)* but that's hopefully not a major issue to anyone.



Note that it has a `using namespace std` somewhere high up. I make no apologies -- this is about solving problems quickly and cleanly, and all the `std::` can, frankly, do one 😎.



#### How to use it to solve puzzles

1. If necessary, copy `template.cpp` to `dayN.cpp`, where `N` is the day of the puzzle you're solving, and replace all the instances of `N` in the code with the appropriate number
2. Copy your puzzle input from the Advent of Code website into a new text file in the `data/` folder, called `dayX.txt`
3. In `main.cpp`, uncomment the appropriate lines to make sure that `run_dayN()` gets called
4. Press `F5` - if all's well you should see a message about your day's test failing
5. Copy all the sample inputs from the puzzle page into your new `dayN.cpp` and hook up a call to `test()` with each of them. **Top Tip:** C++ has multiline string literal support now - you can see the `R"(...)"` block in the template!
6. Write your code to solve the day's puzzle - when all the tests pass, uncomment the `gogogo` line and hopefully you'll get the answer that you can submit to the advent of code page 🤞🎄



#### API Reference

The template comes with a number of helper functions that can save time when solving puzzles. These all live in `harness.h`.

##### Test API 

``` 
  stringlist      - basically std::vector<string> with a few helpers
  READ(str)       - return a _stringlist_ by splitting a string literal into lines
  LOAD(ID)        - return a _stringlist_ by loading a text file from
                    "data/dayID.txt" and splitting it into lines
  LOADSTR(ID)     - like LOAD, but returns a std::string instead of a vector of lines
 
  test(expected, expression)
                  - check that _expression_ evaluates to _expected_ and print the
                    result. used to validate an algorithm against test input
  gogogo(expression)
                  - print out the result of _expression_. 
                    used to run a day's algorithm against the real data
 
  skip()          - moves the global "current part" forward without running any code.
                    perhaps because you solved that day's problem in a different
                    sourcebase (e.g. occasionally there are challenges that are easier
                    to solve using a graphical display than a text one)
                    NOTE: call it twice to skip a whole day
                    
  jumptoday(N)    - moves to global "current day" to _N_ immediately. because after
  					day 20, who has time to be running days 1 thru 19 every time...
                   

  nest and nonono - like _test_ and _gogogo_ but they don't run the expression
  					(typically because it's too slow)
  nestD and nononoD
                  - like _test_ and _gogogo_ but they don't run the expression in
                  	Debug builds (typiclly because it's too slow)
 
  twinkleforever()
                  - have your main() return this for more festive cheer
```

Some examples:

``` c++
    test(3, day1(READ("+1\n-2\n+3\n+1")));
    test<string>("CABDFE", day7(LOAD(7t), 6));
    gogogo(day1(LOAD(1)));
    gogogo(day5(LOADSTR(5)));
    skip("because it needs fancy graphics");
    nest(D11Point{ 232,251,12 }, day11_2(42));
    nonono(day11_2(4455));
    nononoD(day9(string("466 players; last marble is worth 7143600 points")));
```



##### Utilities

```
  stringlist      - basically std::vector<string> with a few helpers
  
  split(str,delimiter)
                  - returns a _stringlist_ of _str_ split by the _delimiter_
                    split("a,b,c", ",")  =>  {"a", "b", "c"}
 
  trim(str, chars=" ")
                  - (in-place) remove leading and trailing characters from _str_
                  	that match any of the characters in _chars_
  ltrim, rtrim    - like _trim_ but only removes characters on the left or right
  					of the string respectively
  trim_copy, ltrim_copy, rtrim_copy
                  - like the above, but return a new string with characters removed,
                  	and leave the original alone
 
  ScopeTimer      - for quick & dirty profiling, prints out how long it takes to run
  					from the moment the object is constructed to the moment
  					it's destructed
```



##### Festive Colour Decorators

If you want to add to the festivities, just sprinkle some colours into `cout`:

```
  std::cout << RED << "o" << YELLOW << "*" << GREEN << "o" << GREY << "*" << RESET;
```

(you need to have called `initcolours()` before this point for these to work)
