# Clatter
A procedural lang with kinda weird expression handling.
The best way to think about this language is all expressions are functions. Pretty much everything can be thought of as a function.
It's kind of lisp in the sense of s-expressions, but c-like syntax for the actual expressions.

Heres an example:
```
print(+(3 5 8 9))
```

And another more complicated one:
```
=(globalTest *(3 6)) //NOTE: all variables declared have to be initialized

//function declaration order shouldn't when calling functions

testThing("hey" 8)

testThing(concat1 concat2):
{
	=(final 0)

	if(<(concat1 concat2))
	{
		+(string(concat1) string(concat2))
	}
	else
	{
		+(string(concat2) string(concat1))
	}

	printsp("result:" final)


	//optional return
	return(1)
}
```

And an example with a for loop

```
for({=(i 0)} {<(i 10)} {=(i +(i 1))})
{
	printsp("hello" i "\n")
}

//if you wanted to just loop forever
for(0 0 0)
{
	print("hello\n")
}
```

Documentation will be available soon.

## Using
Using utf8.h at https://github.com/sheredom/utf8.h, which uses Unlicense and is public domain as well.

## Dependencies
Using a sizeable portion of the C stdlib. Will work to reduce the need in the future.

## License
Clatter is dual-licensed under Unlicense and the MIT license. Choose whichever you want.