# Static Timing Analysis
--------------------------

### Team : LeeHungYeeGiBun
* teamID : cadb113
* Member : 鄧傑方、王志新、徐瑞陽
* Advisor : JIE-HONG JIANG

### Resources
* [Problem Discription](http://cad-contest-2016.el.cycu.edu.tw/Problem_D/default.html)

### Thought
* 先simulate一些pattern，check那些true path candidate是否可以被選，
simulate幾次之後，所剩下的那些，在利用false path detector去解。
* 想不到好的false path detector的解法，大概只能用dfs之後去一一check  (很貴！
* 是否有好的false path解法，可以化成SAT？-> Functional Timing Analysis?
* 對於那些path，是否存在某些structural的特性，可以使得同一組simulate pattern，可以同時為那一組所有path的true path input vector?
