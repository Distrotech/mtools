Name:           mtools
Summary:        mtools, read/write/list/format DOS disks under Unix
Version:        4.0.17
Release:        1
License:        GPLv3+
Group:          Utilities/System
URL:            http://www.gnu.org/software/mtools/
Source:         ftp://ftp.gnu.org/gnu/%{name}/%{name}-%{version}.tar.gz
Buildroot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)


%description
Mtools is a collection of utilities to access MS-DOS disks from GNU
and Unix without mounting them. It supports long file names, OS/2 Xdf
disks, ZIP/JAZ disks and 2m disks (store up to 1992k on a high density
3 1/2 disk).


%prep
%setup -q

./configure \
    --prefix=%{buildroot}%{_prefix} \
    --sysconfdir=/etc \
    --infodir=%{buildroot}%{_infodir} \
    --mandir=%{buildroot}%{_mandir} \
    --enable-floppyd \

%build
make

%clean
echo rm -rf $RPM_BUILD_ROOT
[ X%{buildroot} != X ] && [ X%{buildroot} != X/ ] && rm -fr %{buildroot}

%install
make install
make install-info
strip %{buildroot}%{_bindir}/mtools %{buildroot}%{_bindir}/mkmanifest %{buildroot}%{_bindir}/floppyd
rm %{buildroot}%{_infodir}/dir

%files
%defattr(-,root,root)
%{_infodir}/mtools.info*
%{_mandir}/man1/floppyd.1*
%{_mandir}/man1/floppyd_installtest.1.gz
%{_mandir}/man1/mattrib.1*
%{_mandir}/man1/mbadblocks.1*
%{_mandir}/man1/mcat.1*
%{_mandir}/man1/mcd.1*
%{_mandir}/man1/mclasserase.1*
%{_mandir}/man1/mcopy.1*
%{_mandir}/man1/mdel.1*
%{_mandir}/man1/mdeltree.1*
%{_mandir}/man1/mdir.1*
%{_mandir}/man1/mdu.1*
%{_mandir}/man1/mformat.1*
%{_mandir}/man1/minfo.1*
%{_mandir}/man1/mkmanifest.1*
%{_mandir}/man1/mlabel.1*
%{_mandir}/man1/mmd.1*
%{_mandir}/man1/mmount.1*
%{_mandir}/man1/mmove.1*
%{_mandir}/man1/mpartition.1*
%{_mandir}/man1/mrd.1*
%{_mandir}/man1/mren.1*
%{_mandir}/man1/mshortname.1*
%{_mandir}/man1/mshowfat.1*
%{_mandir}/man1/mtools.1*
%{_mandir}/man5/mtools.5*
%{_mandir}/man1/mtoolstest.1*
%{_mandir}/man1/mtype.1*
%{_mandir}/man1/mzip.1*
%{_bindir}/amuFormat.sh
%{_bindir}/mattrib
%{_bindir}/mbadblocks
%{_bindir}/mcat
%{_bindir}/mcd
%{_bindir}/mclasserase
%{_bindir}/mcopy
%{_bindir}/mdel
%{_bindir}/mdeltree
%{_bindir}/mdir
%{_bindir}/mdu
%{_bindir}/mformat
%{_bindir}/minfo
%{_bindir}/mkmanifest
%{_bindir}/mlabel
%{_bindir}/mmd
%{_bindir}/mmount
%{_bindir}/mmove
%{_bindir}/mpartition
%{_bindir}/mrd
%{_bindir}/mren
%{_bindir}/mshortname
%{_bindir}/mshowfat
%{_bindir}/mtools
%{_bindir}/mtoolstest
%{_bindir}/mtype
%{_bindir}/mzip
%{_bindir}/floppyd
%{_bindir}/floppyd_installtest
%{_bindir}/mcheck
%{_bindir}/mcomp
%{_bindir}/mxtar
%{_bindir}/tgz
%{_bindir}/uz
%{_bindir}/lz
%doc NEWS

%pre
groupadd floppy 2>/dev/null || echo -n ""

%post
if [ -f %{_bindir}/install-info ] ; then
	if [ -f %{_infodir}/dir ] ; then
		%{_bindir}/install-info %{_infodir}/mtools.info %{_infodir}/dir
	fi
	if [ -f %{_infodir}/dir.info ] ; then
		%{_bindir}/install-info %{_infodir}/mtools.info %{_infodir}/dir.info
	fi
fi


%preun
install-info --delete %{_infodir}/mtools.info %{_infodir}/dir.info
if [ -f %{_bindir}/install-info ] ; then
	if [ -f %{_infodir}/dir ] ; then
		%{_bindir}/install-info --delete %{_infodir}/mtools.info %{_infodir}/dir
	fi
	if [ -f %{_infodir}/dir.info ] ; then
		%{_bindir}/install-info --delete %{_infodir}/mtools.info %{_infodir}/dir.info
	fi
fi

%changelog
* Wed Jun 29 2011 Alain Knaff <alain@knaff.lu>
- mbadblocks now takes a list of bad blocks (either as sectors
  or as clusters)
- mbadblocks now is able to do write scanning for bad blocks
- mshowfat can show cluster of specific offset
- Enable mtools to deal with very small sector sizes...
- Fixed encoding of all-lowercase names (no need to mangle
  these)
- Consider every directory entry after an ENDMARK (0x00) to be deleted
- After writing a new entry at end of a directory, be sure to also add
  an ENDMARK (0x00)
- Deal with possibility of a NULL pointer being returned by
  localtime during timestamp conversion
* Sat Apr 16 2011 Alain Knaff <alain@knaff.lu>
- configure.in fixes
- fixed formatting of fat_size_calculation.tex document
- compatibility with current autoconfig versions
- Make it clear that label is limited to 11 characters
- Fixed typo in initialization of FAT32 info sector
* Sun Oct 17 2010 Alain Knaff <alain@knaff.lu>
- Added missing -i option to mshortname
* Sun Oct 17 2010 Alain Knaff <alain@knaff.lu>
- Released v4_0_14:
- New mshortname command
- Fix floppyd for disks bigger than 2 Gig
- Remove obsolete -z flag
- Remove now unsupported AC_USE_SYSTEM_EXTENSIONS
- Fixed output formatting of mdir if MTOOLS_DOTTED_DIR is set
- Mformat now correctly writes backup boot sector
- Fixed signedness of serial number in mlabel
- Fixed buffer size problem in mlabel
- Make mlabel write backup boot sector if FAT32
- Catch situation where both clear and new label are given to mlabel
- Quote filename parameters to scripts
- Mformat: Close file descriptor for boot sector
- Added lzip support to scripts/uz
- Added Tot_sectors option to mformat
- Fixed hidden sector handling in mformat
- Minfo generates mformat command lines containing new -T option
- Mlabel prints error if label too long
* Sun Feb 28 2010 Alain Knaff <alain@knaff.lu>
- Merged Debian patches
* Tue Nov 03 2009 Alain Knaff <alain@knaff.lu>
- Mingw compatibility fixes
