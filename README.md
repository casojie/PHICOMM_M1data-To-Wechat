# PHICOMM_M1data-To-Wechat
使用方法:
1. 将release中的三个文件上传到路由器
2. 设定程序m1wechar为开机运行
3. 设定send_wechar.sh定时运行
* 详细说明：m1wechar会一直运行，一直接收斐讯M1发送来的数据，占用9000端口（可能会与路由固件自身功能冲突，如老毛子的迅雷远程下载功能也使用9000端口），第三步
设定send-wechar.sh定点运行的时间决定着什么时候接收斐讯M1的数据，例如：我设定每天晚上10点运行，那么我就会每天晚上10点收到（昨天晚上10点到今天晚上10点）的数据


改程序由C编写，分为三部分

一.gcc 生成的执行文件，上传路由器中并设置开机运行即可

注意：该程序占有9000端口，请注意路由器固件中使用9000端口的程序（如老毛子的迅雷远程下载）

二.数据存放文本(m1_airdata.txt)，该文件用于存放接收的空气数据
   执行文件接收的数据经过一定的处理。写入该文本

注意：该文件一般无需改动。一二行为MarkDown的格式，关系到发送到微信内容的格式（表格格式）

三.数据发送微信脚步（send_wechar.sh）：把m1_airdata.txt的数据发送到server酱，server酱在发送到我们的微信中
    需要再设定一个定时任务，定时什么时候运行这个程序决定着，何时将数据发送到我们的微信中
    我设定为每天晚上10点运行改脚本，那么10点我就会收到server酱发来的空气数据

---
## 2021-2-17 
* 修改超时次数，尝试解决长时间使用后M1会主动关闭WiFi的情况
---
## 2019-8-27 
* 第一次使用github，还是有点小激动\(≧▽≦)/（程序新手写得烂，大佬多多指教）
该程序基本完善-2019.8.27

程序的基本都已经完善了，基本所有情况都考虑到了
断线重连，错误数据丢弃，调整了下server酱的显示格式
基本不会有什么bug了，挂阿里云跑了大半年
程序没有因错误退出过
理论上交叉编译后可挂路由器上
路由器需要指出“curl”命令即可

但是现在过了大半年没理这程序
我自己都快忘了是怎么写的了
有些地方都忘记为什么这么写了
还请大家自己理解了

## 2019-2-25
* 该程序暂未完善-2019.2.25