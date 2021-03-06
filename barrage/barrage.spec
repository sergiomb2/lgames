# RPM .spec file

Summary: Kill and destroy as many targets as possible within 3 minutes
Name: barrage
Version: 1.0.5
Release: 1
Copyright: GPL
Group: Amusements/Games
Source: %{name}-%{version}.tar.gz
URL: http://lgames.sourceforge.net/index.php?project=Barrage
Prefix: /usr
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRequires:	SDL-devel >= 1.2.0
Requires:	SDL >= 1.2.0
BuildRequires:	SDL_mixer-devel >= 1.2.0
Requires:	SDL_mixer >= 1.2.0

%description
Barrage is a rather violent action game with the objective to kill
and destroy as many targets as possible within 3 minutes. The player
controls a gun that may either fire small or large grenades at
soldiers, jeeps and tanks. It is a very simple gameplay though it is
not that easy to get high scores.

%prep
%setup

%build
./configure --prefix=%{prefix}
make

%install
rm -fR $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -fR $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{prefix}/share/games/*
%{prefix}/bin/*
%{prefix}/share/applications/*
