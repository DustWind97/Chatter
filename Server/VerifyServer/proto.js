//js导入需要模块
const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

//定义个path来拼接
const PROTO_PATH = path.join(__dirname,'message.proto')
//同步加载
const packageDefinition = protoLoader.loadSync(PROTO_PATH,{keepCase:true,longs:String,
    enums:String, defaults:true, oneofs:true
}) 

const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

const message_proto = protoDescriptor.message

//导出模块方便其他使用
module.exports = message_proto