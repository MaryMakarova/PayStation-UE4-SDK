const express = require('express');
const sha1 = require('sha1');

module.exports = class NotificationsRouter {
    constructor(globals) {
        this.globals = globals;

        this.router = express.Router();
        this._setupRouter();
    }

    getRouter() {
        return this.router;
    }

    _setupRouter() {
        this.router.post('/', (req, res, next) => {
            res.statusCode = this.globals.errorStatusCode;

            if (this._handleSignatureVerify(req, res)) {
                _handleNotifications(req, res, JSON.parse(req.rawBody).notification_type);
            }

            res.end();
        });
    }

    _handleNotifications(req, res, notifyType) {
        switch(notifyType) {
            case 'user_validation': 
                this._handleUserValidation(req, res); 
                break;
            case 'payment':
                this._handlePayment(req, res);
                break;
        }
    }

    _handleSignatureVerify(req, res) {
        let extSignature = req.headers.authorization;
        let intSignature = 'Signature ' + sha1(req.rawBody + this.globals.projectSecretKey);
        
        if (extSignature != intSignature) {
            res.statusCode = this.globals.errorStatusCode;
            res.write('{"error":{"code":"INVALID_SIGNATURE"}}');
            return false;
        }

        return true;
    }

    _handleUserValidation(req, res) {
        for (let userId of this.globals.userIdList) {
            if (userId == JSON.parse(req.rawBody).user.id) {
                res.statusCode = this.globals.successStatusCode;
                return;
            } 
        }

        res.write('{"error":{"code":"INVALID_USER"}}');
    }

    _handlePayment(req, res) {
        this.globals.payments.push(JSON.parse(req.rawBody));
        res.statusCode = this.globals.successStatusCode;
    }
};