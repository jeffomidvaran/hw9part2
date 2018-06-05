#!
for f in sqlite3.h tclDecls.h elf.h gmpxx.h H5overflow.h curses.h tcl.h gmp-x86_64.h geos_c.h kdb.h ldap.h mpfr.h cursesw.h expat.h gd.h cblas.h
do
    cp /usr/include/$f ~klefstad/public_html/public/rcp/files/$f
done

cp /usr/share/dict/linux.words ~klefstad/public_html/public/rcp/files/words1
cp /usr/share/dict/linux.words ~klefstad/public_html/public/rcp/files/words2
cp /usr/share/dict/linux.words ~klefstad/public_html/public/rcp/files/words3
