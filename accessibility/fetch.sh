

fetch() {
    local dest=sources

    local url="$1"
    local fname="$dest/$(basename "$url")"

    [ -d "$dest" ] ||
        mkdir "$dest"

    [ -f "$fname" ] ||
        wget -O"$fname" "$url"
}

fetch 'http://ftp.gnome.org/pub/GNOME/sources/atk/2.20/atk-2.20.0.tar.xz'

# git clone 'http://git.freebsoft.org/?p=speechd.git'
fetch 'http://devel.freebsoft.org/pub/projects/speechd/speech-dispatcher-0.8.4.tar.gz'

fetch 'http://ftp.gnome.org/pub/GNOME/sources/gail/1.22/gail-1.22.3.tar.bz2'

fetch 'http://ftp.gnome.org/pub/GNOME/sources/at-spi2-core/2.21/at-spi2-core-2.21.1.tar.xz'

fetch 'http://ftp.gnome.org/pub/GNOME/sources/at-spi2-atk/2.20/at-spi2-atk-2.20.1.tar.xz'
j
fetch 'http://ftp.gnome.org/pub/GNOME/sources/orca/3.21/orca-3.21.1.tar.xz'
