# What works
- simple arithmetic expressions

# what doesn't work
- brackets
- anything other than numbers operations (bitwise operations, hex, binary etc) 


# How does it work
- Takes expression as stdin input
- parses expression and creates token queue
- token queue then gets converted to RPN using shunting yard
- RPN is then evaluated to give output token, value of output token is printed.


# What I want to add next
- improve error handling
- brackets, hex and binary 
- add minimal UI with Qt
- maybe fuzzy testing


# Why am I doing this?
- I don't have a job
- Wanted to see difference between switch based calculator and actual calculator app.