/*
  weatherStation TEC 2016
  Marcos Rodríguez Ovares
  AngularJS Main Configuration
*/
'use strict';

var app = angular.module('weatherStationApp',[
  'ui.router'
])
.config(function($stateProvider, $urlRouterProvider){

  $urlRouterProvider.otherwise('/measures');
  $urlRouterProvider.when('/', '/measures');
  $urlRouterProvider.when('/home', '/measures');

  $stateProvider
  .state('measures',{
      url:'/measures',
      templateUrl:'measures/measures.html',
      controller:'MeasuresController',
      controllerAs:'measures'
    });
});
