import React from "react";

import type { ChildrenProps, SpanProps, TextProps } from "../../../dom/types";
import type { SkiaProps } from "../../processors";

export const Text = ({
  x = 0,
  y = 0,
  ...props
}: SkiaProps<TextProps, "x" | "y">) => {
  return <skText x={x} y={y} {...props} />;
};

export const Span = (props: SkiaProps<SpanProps> & ChildrenProps) => {
  return <skSpan {...props} />;
};
