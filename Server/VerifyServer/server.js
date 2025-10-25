const grpc = require('@grpc/grpc-js')
const message_proto = require('./proto')
const const_module = require('./const')
const {v4:uuidv4} = require('uuid')
const emailModule = require('./email')
const redisModule = require('./redis')

async function GetVerifyCode(call, callback) {
    console.log("email is ", call.request.email)
    try{
        let query_res = await redisModule.GetRedis(const_module.code_prefix+call.request.email);
        console.log("query_res is ", query_res);
        // if(query_res === null){

        // }
        let uniqueId = query_res;
        //未在redis库中找到，则生成一段验证码
        if(query_res == null){
            uniqueId = uuidv4();//利用uuidv4库生成验证码
            if (uniqueId.length > 4) {
                uniqueId = uniqueId.substring(0, 4);
            } 

            //请求的邮箱,并设置过期时间为10分钟=600秒,过期之后会在Redis里会消失
            let bres = await redisModule.SetRedisExpire(const_module.code_prefix+call.request.email, uniqueId,600);//设定为3分钟有效
            if(!bres){
                callback(null, { email:  call.request.email,
                    error:const_module.Errors.RedisErr
                });
                return;
            }
        }

        //在redis库中有验证码，则截取并发送
        console.log("uniqueId is ", uniqueId)
        let text_str =  '您的验证码为: '+ uniqueId +' 请三分钟内完成注册。'
        //发送邮件 服务端邮箱发送给请求的客户端邮箱
        let mailOptions = {
            from: '996834085@qq.com',
            to: call.request.email,
            subject: '验证码',
            text: text_str,
        };
    
        let send_res = await emailModule.SendMail(mailOptions);//等待promise完成，await只能在异步函数中使用
        console.log("send res is ", send_res)

        callback(null, { email:  call.request.email,
            error:const_module.Errors.Success
        }); 
        
 
    }catch(error){
        console.log("catch error is ", error)

        callback(null, { email:  call.request.email,
            error:const_module.Errors.Exception
        }); 
    }
}

function main() {
    var server = new grpc.Server()//启动一个grpc服务监听客户端发送过来的消息
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('127.0.0.1:50051', grpc.ServerCredentials.createInsecure(), () => {
        //server.start()  //最新版本的node.js不需要这一行命令启动了
        console.log('verify grpc server started')        
    })
}

main()