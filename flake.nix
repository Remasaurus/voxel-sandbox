{
  description = "Epic Voxel Sandbox built in C++";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
  };

  outputs = { nixpkgs, ... } @ inputs: 
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in 
    {
    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        cmake
        libGL
        pkg-config

        wayland
        wayland-protocols
        libxkbcommon

        libx11
        libxcursor
        libxrandr
        libxi
        libxinerama
	libGL
      ];
      shellHook = ''
        export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath [ pkgs.wayland pkgs.libxkbcommon]}:$LD_LIBRARY_PATH"
      	echo "Development shell started"
      '';
    };
  };
}
