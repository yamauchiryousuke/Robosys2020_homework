# Robosys2020_homework
**Raspberry Pi 4を用いた3つのLEDを順番に点滅させるデバイスドライバの作成**  
## ・実行環境  
本体：Raspberry Pi 4 Computer Model B 4GB RAM  
OS：Ubuntu Server 20.04.1 LST  
kernel：Linux version 5.4.0  
使用言語：C言語  
## ・実行方法
使用したGPIOピンは23、24、25番ピンである。  
コンパイルと実行方法は以下の通りである。
```
$ cd Robosys2020_homework/myled  
$ maka  
$ sudo insmod myled.ko  
$ sudo chmod 666 /dev/myled0  
$ echo 1 > /dev/myled0  
```
実行すると3つのLEDが順番に点滅し10回ループする。　　
### 実行動画
https://youtu.be/dfBIeGmsfDs
## ・参考URL  
http://embed.hatenablog.com/entry/2017/08/19/002523 
最終閲覧日　2020/12/04
