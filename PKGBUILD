# Maintainer: mooviies <mooviies.aur@gmail.com>
pkgname=tetrominos-git
pkgver=r124.8524f8b
pkgrel=1
pkgdesc='A terminal-based Tetrominos game built with KonsoleGE'
arch=('x86_64')
url='https://github.com/mooviies/Tetrominos'
license=('MIT')
depends=('glibc')
makedepends=('git' 'cmake' 'gcc' 'python')
provides=('tetrominos')
conflicts=('tetrominos')
source=("${pkgname}::git+https://github.com/mooviies/Tetrominos.git")
sha256sums=('SKIP')

pkgver() {
    cd "$srcdir/$pkgname"
    printf 'r%s.%s' "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
    cd "$srcdir/$pkgname"
    git submodule update --init
}

build() {
    cd "$srcdir/$pkgname"
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build
}

package() {
    cd "$srcdir/$pkgname"
    install -Dm755 build/tetrominos "$pkgdir/usr/bin/tetrominos"
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
