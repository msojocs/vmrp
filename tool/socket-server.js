// Socket服务器，不实现任何功能，只是用来看程序像服务发送了什么数据
const net = require('net')
const server = net.createServer(socket => {
    socket.on('data', data => {
        console.info(data)
    })
})
server.listen(80)