# > ***`AltarC Legacy CLI`***
### **Compiles to C++ Code**
##### <span style="color:skyblue">*Written in C*</span>

## `Preface`

This was the original (working) compiler/transpiler for the In-Development Altar language, soon to be renamed to Vire.

This has the code for both the Compiler and its CLI, The API is pretty easy to use and emits C++ code internally, stored in a string which is saved to a file and then it uses g++ to compile to binaries.

---

# `Usage`

> <span style="color:orange"> **Tested only on Windows, should work with Linux distros** </span>

1. Install the Executable from the Releases section on this page.
2. Go to the directory where it is downloaded, and run `altar help` to get a very brief and non-useful help text.
3. **To *run* altar code**, use `altar run filename` where `filename` is your input file's name with extension, for e.g. -
```
$> altar run foo.a
    > g++ foo.a.cpp -o foo.a.exe -O2


$> 
```
should be the output for a given valid `foo.a` file which is present in the current directory.
It adds the .exe extension if you are on Windows, else adds the .out extension.

---

# `Examples`

## 1. <span style="color:skyblue">Fibonacci Sequence</span>
```go
func fib(num:int) -> int
{
    if(num==0)
    {
        return 0;
    }
    else if(num==1) 
    {
        return 1;
    }

    return fib(num-1) + fib(num-2);
}

func main()
{
    echo(fib(69));
}
```
#### **Output**: 
```
117669030460994
```

## 2. <span style="color:skyblue">Loops</span>
```go
func main()
{
    for(var i=0; i<10; i++)
    {
        echo(i);
    }

    while(i>0)
    {
        echo(i);
        --i;
    }
}
```
#### **Output**:
```
0
1
2
3
4
5
4
3
2
1
```

## 3. <span style="color:skyblue">Strings</span>
```go
func main()
{
    var mystr="Hello ";
    
    echo(mystr+"World");
}
```
#### **Output**:
```
Hello World
```

## 4. <span style="color:skyblue">Types</span>

> I am using // as comment because I am using go syntax highlighting

```go
func main()
{
    /- This is a comment
    
    var v1="Hello World" // /- Type inference

    var v2:str; // /- Typed Declaration
    v2="Hello World"; 

    var v3:int = v2; // /- Throws error, declared as `int`, assigned with `str`
}

func foo(num:int) -> int // /- Return type, and arg type
{
    return num+1;
}
```
