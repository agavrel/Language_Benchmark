# Language Benchmark

### Purpose

The purpose of this benchmark is to analyze and discuss advantages of different languages related to:
* Language Readability
* Compile Time
* Binary File Size
* Execution Speed
* Memory consumed


### Program used for Benchmark

The program read a file the following way:
```
EUR;550;JPY
6
AUD;CHF;0.9661
JPY;KRW;13.1151
EUR;CHF;1.2053
AUD;JPY;86.0305
EUR;USD;1.2989
JPY;INR;0.6571
```

First line will have Ticker1;Notional;Ticker2 : How much of Notional you currently have with current currency (Ticker1) and in which desired currency (Ticker2) you want to convert.  

Second line will have the number of different exchange rates contained in the following lines.  

Third line (and following ones) will be formatted the following way:
Ticker1;Ticker2;ExchangeRate
ExchangeRate being how much of Ticker2 you get with one unit of Ticker1.  

The program will return the amount of money we would get if we convert from one currency to another, using the shortest path.  

Hence we use Dijkstra algorithm with a weight of 1 for each node.

The different source codes are found in the respective directories for people who want to take a look and optimize further to make things more fair.

# Analysis

### Language Readability

Overall C# seems to be the best, coding is very easy and readable.  

C++ is also good but requires more attention to memory management.

Java seems a very bad choice compared to both C# and C++ as:  
* There are no overload for operators of BigDecimal object when you are looking to divide or multiply which get really annoying and verbose.  

* Moreover java use .get() to obtain element of an array and .charAt() for the character of a string which also is not helping for the speed of coding.  

* Last, but not least, there is not pre-processor define as in C# and C++ for java



## Compile Time

Result for .NetCore 2.2.3
```
Microsoft (R) Build Engine version 16.1.76+g14b0a930a7 for .NET Core
Copyright (C) Microsoft Corporation. All rights reserved.

  Restore completed in 176.85 ms for ~/Language_Benchmark/C#.NetCore/currency_converter.csproj.
  currency_converter -> ~/Language_Benchmark/C#.NetCore/bin/Release/netcoreapp2.2/linux-x64/currency_converter.dll
  currency_converter -> ~/Language_Benchmark/C#.NetCore/csharp_binary/

real	0m2.462s
user	0m2.828s
sys	0m0.363s
```

Result for CPP:
```
time g++ -D DEBUG=false -std=c++1z currency_converter.cpp -o cpp_binary

real	0m1.044s
user	0m0.976s
sys	0m0.069s
```

```
time g++ -O3 -D DEBUG=false -std=c++1z currency_converter.cpp -o cpp_binary

real	0m1.376s
user	0m1.326s
sys	0m0.050s
```

Result for Java:
```
time ./compile

real	0m0.851s
user	0m2.640s
sys	0m0.082s
```

C++ > Java > .NetCore


### Binary File Size

Result for .NetCore 2.2.3:
```
du -h csharp_binary/
72M	csharp_binary/
```

without the .dll:
```
du -h csharp_binary/currency_converter
108K	csharp_binary/currency_converter
```

Result for CPP (without flag -O3):
```
du -h cpp_binary
188K	cpp_binary
```

Result for CPP (with flag -O3):
```
du -h cpp_binary
56K	cpp_binary
```

Result for Java:
```
du -h *.class
4.0K	CurrencyNode.class
8.0K	Graph.class
4.0K	Main.class
4.0K	Tuple.class
```

The size of the Main class is even lower as it is rounded to multiple of 4, it is actually 3253 bytes which you can check with
```
javap -v -p -c Main
```

But let's say the java binary files are 20.0K

Java > C++ > .NetCore

NB: Java is victorious but it does not take into account the size of the JVM  

### Memory Usage

We will use valgrind to get information about heap allocation:
```
sudo apt-get valgrind
```

And then run it the following way (for C++):
```
valgrind ./cpp_binary ../test_files/example.txt
```

Result:
```
total heap usage: 108 allocs, 107 frees, 97,779 bytes allocated
```

Result for .NetCore:
```
in use at exit: 2,181 bytes in 65 blocks
total heap usage: 70 allocs, 5 frees, 11,493 bytes allocated
```

Result for Java:
```
total heap usage: 71 allocs, 5 frees, 11,509 bytes allocated
```

Java & .NetCore > C++  
C++ used 9 times more memory but that is the price of the efficiency:

### Execution Speed

Result for CPP:
```
time ./cpp_binary ../test_files/example.txt
59033

real	0m0.002s
user	0m0.002s
sys	0m0.000s
```

Result for .NetCore 2.2:
```
time ./csharp_binary/currency_converter ../test_files/example.txt
59033

real	0m0.075s
user	0m0.074s
sys	0m0.012s
```

Result for Java:
```
time java Main ../test_files/example.txt

59033

real	0m0.089s
user	0m0.087s
sys	0m0.026s
```

C++ > .NetCore & Java  

These results should be nuanced as there is a very high cost to start the different.
But it means that for very small programs C++ should be favored over the other ones.

### Conclusion

First Takeaway: You really wonder how people can still use java in 2019. I was curious as well and found this very interesting [topic](https://tech.jonathangardner.net/wiki/Why_Java_Sucks) as well as these [funny quotes](http://harmful.cat-v.org/software/java)

It is funny that java was so bad that Google spent quite some time to adapt it for Android, its mobile OS and then got sued by Sun Corporation. Now Google is switching from Java to Kotlin and this is not surprising at all!!  
Java had smaller size of its binaries (.class) but nowadays the program size does not matter (except for embedded devices, but this is another topic, and the embedded device would need to support the JVM)  

C++ is not as easy to maintain as C# - not to mention that there is not garbage collector that will automatically manage memory - so it makes sense for small businesses to use C# and keep using it.

However banks and financial institutions do use C# a lot, but often there efficiency and performance is a key quality. C# does not reach the level of performance that you can have with cpp but I do agree that it provides easy way to interact with other Microsoft programs (Excel, Word etc) and that is why it was largely adopted in the past.

**My recommendation for any young graduate interested in BackEnd would definitely be to learn C++, maybe Kotlin, Go or Python (no benchmark here unfortunately!), potentially C# if the focus is readability and REST apis, but for God Sake, let's leave Java where it belongs: next to garbage truck.**

### Bonus

The quotes about Java:

> Using Java for serious jobs is like trying to take the skin off a rice pudding wearing boxing gloves. – Tel Hudson  
Of all the great programmers I can think of, I know of only one who would voluntarily program in Java. And of all the great programmers I can think of who don’t work for Sun, on Java, I know of zero. – Paul Graham  
Java is the most distressing thing to happen to computing since MS-DOS. – Alan Kay
Java is, in many ways, C++–. – Michael Feldman  
C++ is history repeated as tragedy. Java is history repeated as farce. – Scott McKay
Java, the best argument for Smalltalk since C++. – Frank Winkler  
Arguing that Java is better than C++ is like arguing that grasshoppers taste better than tree bark. – Thant Tessman  
Java: the elegant simplicity of C++ and the blazing speed of Smalltalk. – Jan Steinman  
Like the creators of sitcoms or junk food or package tours, Java’s designers were consciously designing a product for people not as smart as them. – Paul Graham  
There are undoubtedly a lot of very intelligent people writing Java, better programmers than I will ever be. I just wish I knew why. – Steve Holden  
The more of an IT flavor the job descriptions had, the less dangerous was the company. The safest kind were the ones that wanted Oracle experience. You never had to worry about those. You were also safe if they said they wanted C++ or Java developers. If they wanted Perl or Python programmers, that would be a bit frightening. If I had ever seen a job posting looking for Lisp hackers, I would have been really worried. – Paul Graham  
If you learn to program in Java, you’ll never be without a job! – Patricia Seybold in 1998  
Knowing the syntax of Java does not make someone a software engineer. – John Knight
In the best possible scenario Java will end up mostly like Eiffel but with extra warts because of insufficiently thoughtful early design. – Matthew B Kennel  
Java has been a boon to the publishing industry. – Rob Pike  
The only thing going for java is that it’s consuming trademark namespace. – Boyd Roberts
Java is the SUV of programming tools. A project done in Java will cost 5 times as much, take twice as long, and be harder to maintain than a project done in a scripting language such as PHP or Perl. … But the programmers and managers using Java will feel good about themselves because they are using a tool that, in theory, has a lot of power for handling problems of tremendous complexity. Just like the suburbanite who drives his SUV to the 7-11 on a paved road but feels good because in theory he could climb a 45-degree dirt slope. – Greenspun, Philip  
JAVA truly is the great equalizing software. It has reduced all computers to mediocrity and buggyness. - NASA’s J-Track web site  
C and Java are different in the non-excitability department, though. With C, you don’t get excited about it like you don’t get excited about a good vintage wine, but with Java, you don’t get excited about it like you don’t get excited about taking out the garbage. – Lamont Cranston (aka Jorden Mauro)  
Saying that Java is good because it works on all platforms is like saying anal sex is good because it works on all genders. – Unknown  
java is about as fun as an evening with 300 hornets in a 5m2 room – andguent
If Java had true garbage collection, most programs would delete themselves upon execution. – Robert Sewell  
Java: write once, run away! – Brucee  
Java is a DSL to transform big XML documents into long exception stack traces. – Scott Bellware  
The definition of Hell is working with dates in Java, JDBC, and Oracle. Every single one of them screw it up. – Dick Wall CommunityOne 2007: Lunch with the Java Posse
Java is like a variant of the game of Tetris in which none of the pieces can fill gaps created by the other pieces, so all you can do is pile them up endlessly. – Steve Yegge (2007, Codes Worst Enemy)  
Every modern Java program I see embeds its logic in an exaggerated choose-your-own-adventure model where the decisions of where to jump next occur in one word increments. – deong  
Sufficiently advanced Java is indistinguishable from satire. – @eeppa (Same is true of sufficiently advanced C++)  
Whenever I write code in Java I feel like I’m filling out endless forms in triplicate. – Joe Marshall (aka jrm)  
In the JSR-296 “The intended audience for this snapshot is experienced Swing developers with a moderately high tolerance for pain.” Gil Hova Reply: “Wait. There are Swing developers with low tolerances for pain?”  
