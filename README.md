# What works
- simple arithmetic expressions (BODMAS)
- simple bitshift operations (l_shift, r_shift, and, or)

# what doesn't work
- Hex or binary numbers, exponentials or complex functions 

# How does it work
- Takes expression as stdin input
- parses expression and creates token queue
- token queue then gets converted to RPN using shunting yard
- RPN is then evaluated to give output token, value of output token is printed.


# What I want to add next
- [x] improve error handling
- [x] brackets
- [ ] hex and binary 
- [ ] add minimal UI with Qt
- [ ] maybe fuzzy testing


# Why am I doing this?
- I don't have a job
- Wanted to see difference between switch based calculator and actual calculator app.