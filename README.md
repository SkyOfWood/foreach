# foreach 批量循环工具
```bash
$ foreach -h

Usage:foreach -[hv] [-w threads] [-c circle time] [-s seperator] cmd ...
      -v   Display verbose messages
      -h   Help
      -c   circle:circle time(default value is 1)
      -s   sep:default is space or \t
      -w   threads:thread number(default value is 1)
      -m   set thread max number no limit(default is limit to 40)
      cmd  the command to be executed!
```

```bash
For example
$ date|foreach 'echo #1 #4 #5'

-c 执行次数，默认1次
$ foreach -c 10 'date'

-s 分隔符，默认为空格
$ date|foreach -s: 'echo #2,#3'

-w 设置线程，默认是单线程
$ foreach -c 10 -w 5 'sleep 1 && date +%s'

-m 解除线程上限，默认限制40个线程
$ foreach -c 80 -w 80 -m 'sleep 1 && date +%s'
```