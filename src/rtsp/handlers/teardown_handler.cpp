/*
如果 N 秒内没有任何请求，服务器会：
停止推流
回收 RTP/RTCP socket
删除 Session 记录

一般都是 tcp 链接断开
*/