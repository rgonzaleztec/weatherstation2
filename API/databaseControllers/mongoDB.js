/*
  weatherStation TEC 2016
  Marcos Rodríguez Ovares
  MongoDB Controller
*/
'use strict';

//var host = "localhost";
//var port = 27017;
var dbName = 'dbWheater';

var mongojs = require("mongojs");
var db = mongojs(dbName, ['wheaterMeasures']);

exports.insertWeatherMeasureToDB = function(idStation, altitudeM, altitudeFt, pressure,
  temperatureC, temperatureF, humidity, light, winddir, windspeedmph,rain,dailyRain, callback){
  db.wheaterMeasures.insert({
    idStation : idStation,
    altitudeM : altitudeM,
    altitudeFt : altitudeFt,
    pressure : pressure,
    temperatureC : temperatureC,
    temperatureF : temperatureF,
    humidity : humidity,
    light : light,
    winddir : winddir,
    windspeedmph : windspeedmph,
    rain: rain,
    dailyRain: dailyRain,
    date : new Date().getTime()
  });
};

exports.getWeatherDataDB = function(idStation, limit, callback){
  limit = parseInt(limit);
  db.wheaterMeasures.find({'idStation':idStation}).sort({_id:-1}).limit(limit,function(error, docs){
    callback(error,docs);
  });
};
