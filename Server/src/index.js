const express = require('express');

const TokenRouter           = require('./tokenRouter');
const NotificationsRouter   = require('./notificationsRouter');
const PaymentRouter         = require('./paymentRouter');
const UserRouter            = require('./userRouter');

const Globals = {
    port: 3333,
    apiKey: 'IntutXFyKb3HKlLt',
    merchantId: '43043',
    projectId: 24536,
    projectSecretKey: '0HyiqVWXx4gFonSN',
    successStatusCode: 200,
    errorStatusCode: 400,
    userIdList: new Set(),
    payments: []
};

const app = express();

// need to parse post body as raw string
app.use((req, res, next) => {
    req.rawBody = '';
    req.setEncoding('utf8');
  
    req.on('data', function(chunk) { 
      req.rawBody += chunk;
    });
  
    req.on('end', function() {
      next();
    });
});

const tokenRouter           = new TokenRouter(Globals);
const notificationsRouter   = new NotificationsRouter(Globals);
const paymentRouter         = new PaymentRouter(Globals);
const userRouter            = new UserRouter(Globals);

app.use('/token', tokenRouter.getRouter());
app.use('/notifications', notificationsRouter.getRouter());
app.use('/payment', paymentRouter.getRouter());
app.use('/user', userRouter.getRouter());

app.listen(Globals.port);