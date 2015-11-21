'use strict';

var gulp = require('gulp');
var sequence = require('gulp-sequence');
var fs = require('fs');
var argv = require('minimist')(process.argv.slice(2), {
  string: 'target'
});
var task = argv._[0] || 'default';
var target = argv.target || task === 'build' ? 'production' : 'development';
var config = require('./gulp/config.js');
console.log('\n' +
  '===================================================\n' +
  ' Task: %s\n' +
  ' Target: %s\n' +
  '===================================================\n', task, target
);
fs.readdirSync('./gulp/').forEach(function (file) {
  if (file !== 'config.js') {
    require('./gulp/' + file)(target, config);
  }
});
gulp.task('dev', ['scripts', 'styles']);
gulp.task('build', sequence('linters', ['scripts', 'styles'], 'git-add'));
gulp.task('default', ['dev']);
