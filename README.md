# Xsolla Plugin

### Install 
Clone or download and unpack plugin to `{YourProject}/Plugins/`

### Setting up
Open plugin settings `Setting > Project Settings > Xsolla Plugin > General`, then set `merchant id`, `project id` and `api key`. If you want to use shop in sandbox mode, check `Sandbox Mode` checkbox. 

### How to use in blueprint
To get the shop reference in blueprint class, call `Get Xsolla Plugin Shop`. The function returns `Xsolla Plugin Shop` reference.

`Xsolla Plugin Shop` has 2 available blueprint functions:
* `Create` - Gets shop token, set default properties and creates widget with shop content. Takes shop size and delegates for payment succeeded|canceled|failed as parameters. 
* `Set(Num|Bool|String)Property` - Set properties in json, which is sent as token parameters, see [token](https://developers.xsolla.com/ru/api_v2.html#token). Should be called before `Create` function.

### Example
![Blueprint example](https://github.com/SomeInc/XsollaUE4Plugin/blob/master/blueprint_example.png)

### How to use in c++

