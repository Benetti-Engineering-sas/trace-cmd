# SPDX-License-Identifier: LGPL-2.1
#
# Copyright (c) 2023 Daniel Wagner, SUSE LLC

# input text file: man page section

sources = {
    'trace-cmd.1.txt': '1',
    'trace-cmd-agent.1.txt': '1',
    'trace-cmd-check-events.1.txt': '1',
    'trace-cmd-clear.1.txt': '1',
    'trace-cmd-convert.1.txt': '1',
    'trace-cmd.dat.v6.5.txt': '5',
    'trace-cmd.dat.v7.5.txt': '5',
    'trace-cmd-dump.1.txt': '1',
    'trace-cmd-extract.1.txt': '1',
    'trace-cmd-hist.1.txt': '1',
    'trace-cmd-list.1.txt': '1',
    'trace-cmd-listen.1.txt': '1',
    'trace-cmd-mem.1.txt': '1',
    'trace-cmd-options.1.txt': '1',
    'trace-cmd-profile.1.txt': '1',
    'trace-cmd-record.1.txt': '1',
    'trace-cmd-report.1.txt': '1',
    'trace-cmd-reset.1.txt': '1',
    'trace-cmd-restore.1.txt': '1',
    'trace-cmd-set.1.txt': '1',
    'trace-cmd-show.1.txt': '1',
    'trace-cmd-snapshot.1.txt': '1',
    'trace-cmd-split.1.txt': '1',
    'trace-cmd-stack.1.txt': '1',
    'trace-cmd-start.1.txt': '1',
    'trace-cmd-stat.1.txt': '1',
    'trace-cmd-stop.1.txt': '1',
    'trace-cmd-stream.1.txt': '1',
}

confdir = meson.current_source_dir() + '/../'
top_source_dir = meson.current_source_dir() + '/../../'

#
# For asciidoc ...
#   -7.1.2,     no extra settings are needed.
#    8.0-,      set ASCIIDOC8.
#

#
# For docbook-xsl ...
#   -1.68.1,         set ASCIIDOC_NO_ROFF? (based on changelog from 1.73.0)
#    1.69.0,         no extra settings are needed?
#    1.69.1-1.71.0,  set DOCBOOK_SUPPRESS_SP?
#    1.71.1,         no extra settings are needed?
#    1.72.0,         set DOCBOOK_XSL_172.
#    1.73.0-,        set ASCIIDOC_NO_ROFF
#

#
# If you had been using DOCBOOK_XSL_172 in an attempt to get rid
# of 'the ".ft C" problem' in your generated manpages, and you
# instead ended up with weird characters around callouts, try
# using ASCIIDOC_NO_ROFF instead (it works fine with ASCIIDOC8).
#

if get_option('asciidoctor')
    asciidoc = find_program('asciidoctor')
    asciidoc_extra  = ['-a', 'compat-mode']
    asciidoc_extra += ['-I.']
    asciidoc_extra += ['-r', 'asciidoctor-extensions']
    asciidoc_extra += ['-a', 'mansource=libtraceevent']
    asciidoc_extra += ['-a', 'manmanual="libtraceevent Manual"']
    asciidoc_html = 'xhtml5'
else
    asciidoc = find_program('asciidoc')
    asciidoc_extra  = ['--unsafe']
    asciidoc_extra += ['-f', confdir + 'asciidoc.conf']
    asciidoc_html = 'xhtml11'

    r = run_command(asciidoc, '--version', check: true)
    v = r.stdout().strip()
    if v.version_compare('>=8.0')
        asciidoc_extra += ['-a', 'asciidoc7compatible']
    endif
endif

manpage_xsl = confdir + 'manpage-normal.xsl'

if get_option('docbook-xls-172')
    asciidoc_extra += ['-a', 'libtraceevent-asciidoc-no-roff']
    manpage_xsl = confdir + 'manpage-1.72.xsl'
elif get_option('asciidoc-no-roff')
    # docbook-xsl after 1.72 needs the regular XSL, but will not
    # pass-thru raw roff codes from asciidoc.conf, so turn them off.
    asciidoc_extra += ['-a', 'libtraceevent-asciidoc-no-roff']
endif

xmlto = find_program('xmlto')
xmlto_extra = []

if get_option('man-bold-literal')
    xmlto_extra += ['-m ', confdir + 'manpage-bold-literal.xsl']
endif

if get_option('docbook-suppress-sp')
    xmlto_extra += ['-m ',  confdir + 'manpage-suppress-sp.xsl']
endif

gen = generator(
    asciidoc,
    output: '@BASENAME@.xml',
    arguments: [
        '-b', 'docbook',
        '-d', 'manpage',
        '-a', 'libtraceevent_version=' + meson.project_version(),
        '-o', '@OUTPUT@']
        + asciidoc_extra
        +  ['@INPUT@'])

man = []
html = []
foreach txt, section : sources
    # build man page(s)
    xml = gen.process(txt)
    man += custom_target(
        txt.underscorify() + '_man',
        input: xml,
        output: '@BASENAME@.' + section,
        command: [
            xmlto,
            '-m', manpage_xsl,
            'man',
            '-o', '@OUTPUT@']
            + xmlto_extra
            + ['@INPUT@'])

    # build html pages
    html += custom_target(
        txt.underscorify() + '_html',
        input: txt,
        output: '@BASENAME@.html',
        command: [
            asciidoc,
            '-b', asciidoc_html,
            '-d', 'manpage',
            '-a', 'libtraceevent_version=' + meson.project_version(),
            '-o', '@OUTPUT@']
            + asciidoc_extra
            + ['@INPUT@'])
endforeach

# Install path workaround because:
#
# - xmlto might generate more than one file and we would to tell meson
#   about those output files. We could figure out which files are generated
#   (see sed match in check-manpages.sh).
#
# - The man page generation puts all the generated files under sub dirs
#   and it's not obvious how to tell Meson it should not do this without
#   causing the install step to fail (confusion where the generated files
#   are stored)
#
# - The documentation build is not part of the 'build' target. The user
#   has explicitly to trigger the doc build. Hence the documentation is
#   not added to the 'install' target.
#
# Thus just use a plain old shell script to move the generated files to the
# right location.

conf = configuration_data()
conf.set('SRCDIR', meson.current_build_dir())
conf.set('MANDIR', mandir)
conf.set('HTMLDIR', htmldir)
configure_file(
    input: 'install-docs.sh.in',
    output: 'install-docs.sh',
    configuration: conf)

meson.add_install_script(
    join_paths(meson.current_build_dir(), 'install-docs.sh'))
