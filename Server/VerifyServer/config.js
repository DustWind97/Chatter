const fs = require('fs');//文件系统

let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
//解析json文件
//email信息
let email_user = config.email.user;
let email_pass = config.email.passwd;
//MySQL信息
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
//Redis信息
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;

//服务前缀（验证服务）
let code_prefix = "code_";

//将这些功能封装成模块导出
module.exports = {email_pass, email_user, mysql_host, mysql_port,redis_host, redis_port, redis_passwd, code_prefix}