'use strict';

var gulp = require('gulp');
var compass = require('gulp-compass');
var notify = require('gulp-notify');
var clean = require('gulp-clean');
var ignore = require('gulp-ignore');
var imagemin = require('gulp-imagemin');
var pngquant = require('imagemin-pngquant');
var rename = require('gulp-rename');
var replace = require('gulp-replace');
module.exports = function (target, options) {
  var srcDir = options.srcDir;
  var destDir = options.destDir;
  var isProd = target === 'production';
  var cacheBuster = Date.now();

  // forcing compass to regenerate the css file
  // it does regenerate only when scss changes are detected
  // but when the following config is changed it doesn't
  gulp.task('stylesClean', function () {
    return gulp.src([destDir + '/css/styles.css', destDir + '/css/styles.css.map'], {
      read: false
    }).pipe(clean());
  });

  /*jshint camelcase: false */
  gulp.task('stylesCompass', ['stylesClean'], function () {
    return gulp.src(srcDir + '/sass/styles.scss')
      .pipe(compass({
        environment: isProd ? target : 'development',
        style: isProd ? 'compressed' : 'expanded',
        sourcemap: isProd,
        relative: true,
        css: destDir + '/css',
        sass: srcDir + '/sass',
        generated_images_path:  destDir + '/images',
        image: srcDir + '/images',
        comments: true
      })).on('error', notify.onError({
        title: 'Sass compile error',
        message: '<%= error.message %>'
      }));
  });

  gulp.task('spritesRename', ['stylesCompass'], function () {
    return gulp.src(destDir + '/images/**/*')
      .pipe(ignore.include(/\-[a-z0-9]{11}\.png/))
      .pipe(ignore.exclude(/\/sprite-.+\.png/))
      .pipe(rename(function (path) {
        path.basename = path.basename.replace(/^(.+)-[a-z0-9]{11}$/, 'sprite-$1');
      }))
      .pipe(gulp.dest(destDir + '/images'));
  });

  gulp.task('spriteReplace', ['spritesRename'], function () {
    gulp.src(destDir + '/css/styles.css')
      .pipe(replace(/(['"\/])([^'"\/]+)-[a-z0-9]{11}.png/g, '$1sprite-$2.png?cb=' + cacheBuster))
      .pipe(gulp.dest(destDir + '/css'));
  });

  // clean up compass generated images
  gulp.task('spritesCleanup', ['spriteReplace'], function () {
    return gulp.src(destDir + '/images/**/*')
      .pipe(ignore.include(/\-[a-z0-9]{11}\.png/))
      .pipe(ignore.exclude(/\/sprite-.+\.png/))
      .pipe(clean());
  });

  gulp.task('images', ['spritesCleanup'], function () {
    return gulp.src(destDir + '/images/**/*')
      .pipe(ignore.exclude(new RegExp('images\/' +
        '(icons|main-menu)'
      )))
      .pipe(gulp.dest(destDir + '/images'));
  });

  gulp.task('styles', [isProd ? 'images' : 'stylesCompass']);
};
