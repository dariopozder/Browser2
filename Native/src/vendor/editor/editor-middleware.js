function CodeEditor() {
        var options = {
            'editorEl': 'code-editor',
            'code': '',
            'theme': 'monokai',
            'fontSize': '16px',
            'mode': 'svg'
        };
        var app = this;
        app.aceEdit;
        app.history = [];
        app.currentFile = false;
        app.mode = function (modeString) {
            app.aceEdit.getSession().setMode('ace/mode/' + modeString);
            options.mode = modeString;
        };
        app.theme = function (themeString) {
            app.aceEdit.setTheme('ace/theme/' + themeString);
        };
        app.get = function () {
            return toJson(app.aceEdit.getValue());
        };
        app.set = function (newCode) {
            if (newCode.length / 1024 > 5000) {
                console.log('limit is 5mB of code');
                return;
            }
            if (app.currentFile.mode == 'javascript') {
                app.aceEdit.setValue(fromJson(newCode), -1);
            } else {
                app.aceEdit.setValue(newCode, -1);
            }
            app.aceEdit.clearSelection();
        };
        app.resize = function () {
            //toolbar height and margin
            app.aceEdit.container.style.height = window.innerHeight - document.querySelector('.toolbar').offsetHeight - 5 + 'px';
            app.aceEdit.resize();
        };
        app.fold = function () {
            app.currentFile.session.foldAll(1, app.currentFile.session.getAllLines().length);
        };
        app.createSession = function (data, mode) {
            return ace.createEditSession(data, 'ace/mode/' + mode);
        };
        app.beautify = function () {
            var beautify = ace.require('ace/ext/beautify');
            // get reference to extension
            beautify.beautify(app.aceEdit.session);
        };
        app.search = function () {
            var search = ace.require('src/ext/searchbox');
            // get reference to extension
            app.aceEdit.$search.find('function');
            //  beautify.beautify(app.aceEdit.session);
            app.aceEdit.execCommand('find');    //should display the searchBox. You can also use
                                                // app.aceEdit.searchBox.show();
                                                //editor.searchBox.hide();
        };
        app.selectAll = function () {
            app.aceEdit.selectAll();
            app.aceEdit.container.focus();    // app.aceEdit.container.select();
        };
        function events() {
            if (!window.parent) {
                window.warp = {
                    inspect: function (s) {
                        console.log(s);
                    }
                };
            } else {
                window.warp = window.parent.warp;
            }
            if (!window.main)
                window.main = {};
            main.envVars = { 'url': 'https://bemjax.com/' };
            function keyDown(evt) {
                if (evt.ctrlKey || evt.metaKey) {
                    switch (String.fromCharCode(evt.which).toLowerCase()) {
                    case 's':
                        event.preventDefault();
                        if (warp.editor.file.drive) {
                            warp.editor.save();
                        } else {
                            warp.editor.commit();
                        }
                        break;
                    case 'f':
                        event.preventDefault();
                        //alert('ctrl-f');
                        //warp.editor.show();
                        app.search();
                        break;
                    case 'g':
                        // event.preventDefault();
                        break;
                    case 'q':
                        event.preventDefault();
                        warp.inspect(warp.apps.editor.aceEdit.getSelectedText());
                        break;
                    case 'a':
                        event.preventDefault();
                        //warp.apps.browser.win.show();
                        //alert('ctrl-g');
                        break;
                    case 'e':
                        event.preventDefault();
                        warp.openConsole('warp.explicit(\'\');');
                        break;
                    }
                }
                //f12
                if (evt.keyCode == 123 || evt.keyCode == 121) {
                    evt.preventDefault();
                    warp.console.show();
                }
                //f5
                if (evt.keyCode == 21 || evt.keyCode == -96) {
                    evt.preventDefault();
                }
                // Tab key
                if (evt.keyCode == 9) {
                }
                if (evt.keyCode == 116) {
                }
                // ctrl key
                if (evt.keyCode == 17) {
                }    // Backspace key
                else if (evt.keyCode == 8) {
                }    // Delete key
                else if (evt.keyCode == 46) {
                }    // Left/right arrow keys
                else if (evt.keyCode == 37) {
                } else if (evt.keyCode == 39) {
                }
            }
            document.body.addEventListener('keydown', keyDown);
            /*    window.addEventListener('contextmenu', e => e.preventDefault());
        document.body.addEventListener('context', function (e) {
            e.preventDefault();
        });
      */
            var contextFunctions = {
                'dsdsd': function () {
                    warp.inspect(app.aceEdit);
                }
            };
            if (!window.parent)
                window.parent.main.context.add(editor.container, contextFunctions, 'Testis');
            app.aceEdit.setFontSize(14);
            editor.container.style.lineHeight = 2;
            editor.renderer.updateFontSize();
        }
        function init() {
            // trigger extension
            var langTools = ace.require('ace/ext/language_tools');
            //  ace.config.set('basePath', '/resources/js/vendor');
            //   ace.config.set('modePath', '/resources/js/vendor');
            ace.config.set('basePath', '/editor/src-min');
            ace.config.set('modePath', '/editor/src-min');
            ace.config.set('themePath', '/editor/src-min');
            app.aceEdit = ace.edit('editor');
            //just because tern plugin is very badly programmed
            //it supposes that we all cal our instances "editor"
            window.editor = app.aceEdit;
            app.aceEdit.getSession().setUseWrapMode(true);
            app.aceEdit.setOptions({
                fontSize: options.fontSize,
                enableBasicAutocompletion: true,
                enableSnippets: true,
                enableLiveAutocompletion: true
            });
            // Create tern server
         //   var TernServer = ace.require('ace/tern/server').TernServer;

        //    var defs = [
         //       ternBrowserDef,
        //        ternEcma5Def
        //    ];
           // window.ternPlugin = new TernServer({ defs: defs });
          //  window.ternPlugin.server.addFile('testingeditor.js', 'function iii(a){ alert(a) } warp.ies = function(x){this.xyy = [32,67];this.x=12;}');
            app.mode(options.mode);
            app.set(options.code);
            // Tern Completion
   
         //   langTools.addCompleter(ternPlugin);
         
            var bCompleter = {
                getCompletions: function (editor, session, pos, prefix, callback) {
                    var wordList = [{
                            'name': 'warp',
                            'value': 'warp',
                            'score': 800,
                            'meta': 'warp IDE'
                        }];
                    if (prefix.length === 0) {
                        callback(null, wordList);
                        return;
                    }
                    //  if (prefix === 'b' || prefix === 'ide') {
                    //           alert(prefix)
                    wordList = [
                        {
                            'name': 'warp',
                            'value': 'warp.inspect()',
                            'score': 800,
                            'meta': 'b IDE'
                        },
                        {
                            'name': 'inspected',
                            'value': 'inspected',
                            'score': 800,
                            'meta': 'b IDE'
                        },
                        {
                            'name': 'item({..})',
                            'value': 'item({ \ndata : null, \ntext : ""})',
                            'score': 800,
                            'meta': 'b IDE'
                        },
                        {
                            'name': 'slot',
                            'value': 'slot({ \ntext : "",\nbrands : [""],\nrecive : function(){ \n\n}})',
                            'score': 800,
                            'meta': 'b IDE'
                        },
                        {
                            'name': 'inspect()',
                            'value': 'inspect()',
                            'score': 800,
                            'meta': 'b IDE'
                        }
                    ];
                    //  }
                    callback(null, wordList);
                }
            };
          //  langTools.addCompleter(bCompleter);
            var search = ace.require('src/ext/searchbox');
            // Tern Tooltip
          //  var TernTooltip = ace.require('ace/tern/tern_tooltip').TernTooltip;
         //   editor.ternTooltip = new TernTooltip(editor, ternPlugin);
            app.EditSession = ace.require('ace/edit_session').EditSession;
            var tester = new app.EditSession('toast("hello from warp");\nwarp.inspect(warp);', 'ace/mode/javascript');
            app.aceEdit.setSession(tester);
            app.mode('javascript');
            events();
            if (window.warp && window.warp.editor)
                window.warp.editor.FPSTest = app.test;
            return app;
        }
        return init();
    }
    document.addEventListener('DOMContentLoaded', function (event) {
        window.codeEditor = new CodeEditor();
    });