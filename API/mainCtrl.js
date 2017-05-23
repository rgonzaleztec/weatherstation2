/*
  wheaterStation TEC 2016
  Marcos Rodríguez Ovares
  Main server controller
*/

'use strict';

var mongoDBCtrl = require('./databaseControllers/mongoDB.js');

exports.insertWeatherMeasure = function(request, response){
  var idStation = request.params.idStation;
  var altitudeM = request.params.altitudeM;
  var altitudeFt = request.params.altitudeFt;
  var pressure = request.params.pressure;
  var temperatureC = request.params.temperatureC;
  var temperatureF = request.params.temperatureF;
  var humidity = request.params.humidity;
  var light = request.params.light;
  var winddir = request.params.winddir;
  var windspeedmph = request.params.windspeedmph;
  var rain = request.params.rain;
  var dailyRain = request.params.dailyRain;

  mongoDBCtrl.insertWeatherMeasureToDB(idStation, altitudeM, altitudeFt, pressure,
    temperatureC, temperatureF, humidity, light, winddir, windspeedmph,rain, dailyRain);

  console.log('Data send to MongoDB');
  //response.json({'status':'OK'});
};


exports.getWeatherData = function(request, response){
  var idStation = request.params.idStation;
  var limit = request.params.limit;
  mongoDBCtrl.getWeatherDataDB(idStation, limit, function(err, docs){
      if(err){
        response.json({'err':true, 'data':docs, 'message':'MongoDB error'});
      }else {
        response.json({'err':false, 'data':docs, 'message':'success'});
      }
  });
};
