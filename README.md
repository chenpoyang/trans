trans
=====

	file transmitter

usage
=====
    compile:
        make

	run:
	    1):首先启动服务器                ---->   ./server, 默认服务务器为:127.0.0.1, 端口为8888
	    2):启动客户端, 连接到服务器上    ---->   ./client 127.0.0.1 8888
	    3):若想启动多个客户端, 重复2)即可

file transfer protocol
======================
    path file_bytes \r\n
    \r\n
    file stream

progress bar
===========
    由于在本地环回测试, 对于传输小的文件, 速度很快, 不易看清进度条的详细信息, 如进度, 发送平均速度
    所以请传输200MB左右的文件， 便于显示.

关于跨平台
=========
    本程序除用了 posix 的pthread外, 实现全部用 ANSI C实现.
