const express = require('express');

const TokenRouter           = require('./tokenRouter');
const NotificationsRouter   = require('./notificationsRouter');
const PaymentRouter         = require('./paymentRouter');

const Globals = {
    port: 3333,
    apiKey: 'IntutXFyKb3HKlLt',
    merchantId: '43043',
    projectSecretKey: '0HyiqVWXx4gFonSN',
    successStatusCode: 200,
    errorStatusCode: 400,
    userIdList: ['1'],
    payments: []
};

const app = express();

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

const tokenRouter = new TokenRouter(Globals);
const notificationsRouter = new NotificationsRouter(Globals);
const paymentRouter = new PaymentRouter(Globals);

app.use('/token', tokenRouter.getRouter());
app.use('/notifications', notificationsRouter.getRouter());
app.use('/payment', paymentRouter.getRouter());

app.listen(Globals.port);