function u(code) {
    var options = UglifyJS.defaults({mangle:true});
    UglifyJS.base54.reset();

    var toplevel = null
    toplevel = UglifyJS.parse(code, { filename : '?' , toplevel : toplevel })

    toplevel.figure_out_scope(options.mangle);
    toplevel.compute_char_frequency(options.mangle);
    toplevel.mangle_names(options.mangle);

    var output = {};
    var stream = UglifyJS.OutputStream(output);
    toplevel.print(stream);
    return stream + "";
}
